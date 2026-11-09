// MainWindow.cpp
#include "MainWindow.h"

#include "ControlBar.h"
#include "InstructionSetDialog.h"
#include "MemoryPanel.h"
#include "ProgramListPanel.h"
#include "RegisterPanel.h"
#include "ScreenPanel.h"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeySequence>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QSettings>
#include <QSignalBlocker>
#include <QSplitter>
#include <QStatusBar>
#include <QUrl>
#include <QVBoxLayout>

namespace {
constexpr int kMaxRecentFiles = 8;
const char* kOrganization = "Vole";
const char* kApplication  = "VoleGui";
} // namespace

// ============================================================================
// construction
// ============================================================================

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Vole Machine Simulator");
    setAcceptDrops(true);

    buildUi();
    buildMenus();
    loadSettings();
    connectSignals();

    runTimer.setSingleShot(false);
    runTimer.setTimerType(Qt::PreciseTimer);   // steadier ticks than the default coarse timer
    runTimer.setInterval(controlBar->stepDelayMs());

    before = MachineSnapshot::take(machine);
    refresh();
}

void MainWindow::buildUi()
{
    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    controlBar = new ControlBar(central);
    layout->addWidget(controlBar);

    programList   = new ProgramListPanel;
    memoryPanel   = new MemoryPanel;
    registerPanel = new RegisterPanel;
    screenPanel   = new ScreenPanel;

    auto* rightSplitter = new QSplitter(Qt::Vertical);
    rightSplitter->addWidget(registerPanel);
    rightSplitter->addWidget(screenPanel);
    rightSplitter->setStretchFactor(1, 1);

    auto* mainSplitter = new QSplitter(Qt::Horizontal, central);
    mainSplitter->addWidget(programList);
    mainSplitter->addWidget(memoryPanel);
    mainSplitter->addWidget(rightSplitter);
    mainSplitter->setSizes({380, 540, 320});
    mainSplitter->setStretchFactor(1, 1);
    layout->addWidget(mainSplitter, 1);

    setCentralWidget(central);

    stateLabel = new QLabel(this);
    pcLabel    = new QLabel(this);
    stepsLabel = new QLabel(this);
    statusBar()->addPermanentWidget(stateLabel);
    statusBar()->addPermanentWidget(pcLabel);
    statusBar()->addPermanentWidget(stepsLabel);

    resize(1250, 780);
}

void MainWindow::buildMenus()
{
    // ---- File ----
    QMenu* fileMenu = menuBar()->addMenu("&File");

    QAction* openAction = fileMenu->addAction("&Open...");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::onOpen);

    recentMenu = fileMenu->addMenu("Open &Recent");

    restartAction = fileMenu->addAction("Re&start program");
    restartAction->setShortcut(QKeySequence("Ctrl+Shift+R"));
    connect(restartAction, &QAction::triggered, this, &MainWindow::onRestart);

    fileMenu->addSeparator();
    QAction* quitAction = fileMenu->addAction("&Quit");
    quitAction->setShortcut(QKeySequence::Quit);
    connect(quitAction, &QAction::triggered, this, &QWidget::close);

    // ---- Run ----
    QMenu* runMenu = menuBar()->addMenu("&Run");

    stepAction = runMenu->addAction("&Step");
    stepAction->setShortcut(QKeySequence(Qt::Key_F10));
    connect(stepAction, &QAction::triggered, this, &MainWindow::onStep);

    runAllAction = runMenu->addAction("Run &All");
    runAllAction->setShortcut(QKeySequence(Qt::Key_F5));
    connect(runAllAction, &QAction::triggered, this, &MainWindow::onRunAll);

    runAction = runMenu->addAction("&Run / Pause");
    runAction->setShortcut(QKeySequence(Qt::Key_F6));
    connect(runAction, &QAction::triggered, this, [this]() {
        onRunToggled(uiState != UiState::Running);
    });

    runMenu->addSeparator();
    resetAction = runMenu->addAction("Re&set machine");
    resetAction->setShortcut(QKeySequence("Ctrl+R"));
    connect(resetAction, &QAction::triggered, this, &MainWindow::onReset);

    // ---- View ----
    QMenu* viewMenu = menuBar()->addMenu("&View");
    QMenu* baseMenu = viewMenu->addMenu("&Number base");

    auto* baseGroup = new QActionGroup(this);
    baseGroup->setExclusive(true);

    struct BaseEntry { const char* text; const char* name; DisplayBase base; };
    const BaseEntry entries[] = {
        {"&Hexadecimal", "baseHex",     DisplayBase::Hex},
        {"&Decimal",     "baseDecimal", DisplayBase::Decimal},
        {"&Binary",      "baseBinary",  DisplayBase::Binary},
    };
    for (const BaseEntry& e : entries) {
        QAction* a = baseMenu->addAction(e.text);
        a->setObjectName(e.name);
        a->setCheckable(true);
        a->setData(static_cast<int>(e.base));
        a->setChecked(e.base == displayBase);
        baseGroup->addAction(a);
    }
    connect(baseGroup, &QActionGroup::triggered, this, &MainWindow::onDisplayBaseSelected);

    darkModeAction = viewMenu->addAction("&Dark mode");
    darkModeAction->setCheckable(true);
    connect(darkModeAction, &QAction::toggled, this, &MainWindow::onThemeToggled);

    // ---- Help ----
    QMenu* helpMenu = menuBar()->addMenu("&Help");
    QAction* setAction = helpMenu->addAction("&Instruction set...");
    connect(setAction, &QAction::triggered, this, &MainWindow::onShowInstructionSet);
    QAction* aboutAction = helpMenu->addAction("&About");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
}

void MainWindow::connectSignals()
{
    // control bar
    connect(controlBar, &ControlBar::loadClicked,    this, &MainWindow::onOpen);
    connect(controlBar, &ControlBar::stepClicked,    this, &MainWindow::onStep);
    connect(controlBar, &ControlBar::runAllClicked,  this, &MainWindow::onRunAll);
    connect(controlBar, &ControlBar::runToggled,     this, &MainWindow::onRunToggled);
    connect(controlBar, &ControlBar::restartClicked, this, &MainWindow::onRestart);
    connect(controlBar, &ControlBar::resetClicked,   this, &MainWindow::onReset);
    connect(controlBar, &ControlBar::speedChanged,   this, &MainWindow::onSpeedChanged);

    connect(&runTimer, &QTimer::timeout, this, &MainWindow::onTimerTick);

    // memory panel
    connect(memoryPanel, &MemoryPanel::memoryEdited, this, &MainWindow::onMemoryEdited);
    connect(memoryPanel, &MemoryPanel::editRejected, this, &MainWindow::onEditRejected);
    connect(memoryPanel, &MemoryPanel::setProgramCounterRequested,
            this, &MainWindow::onSetProgramCounterRequested);

    // register panel
    connect(registerPanel, &RegisterPanel::registerEdited,       this, &MainWindow::onRegisterEdited);
    connect(registerPanel, &RegisterPanel::programCounterEdited, this, &MainWindow::onProgramCounterEdited);
    connect(registerPanel, &RegisterPanel::editRejected,         this, &MainWindow::onEditRejected);

    // program list
    connect(programList, &ProgramListPanel::breakpointToggled, this, &MainWindow::onBreakpointToggled);
    connect(programList, &ProgramListPanel::setProgramCounterRequested,
            this, &MainWindow::onSetProgramCounterRequested);
}

// ============================================================================
// settings
// ============================================================================

void MainWindow::loadSettings()
{
    QSettings settings(kOrganization, kApplication);

    recentFiles = settings.value("recentFiles").toStringList();

    const bool dark = settings.value("darkTheme", false).toBool();
    ThemeManager::apply(dark ? Theme::Dark : Theme::Light);
    if (darkModeAction) {
        QSignalBlocker blocker(darkModeAction);
        darkModeAction->setChecked(dark);
    }

    const int baseValue = settings.value("displayBase", static_cast<int>(DisplayBase::Hex)).toInt();
    switch (baseValue) {
    case static_cast<int>(DisplayBase::Decimal): displayBase = DisplayBase::Decimal; break;
    case static_cast<int>(DisplayBase::Binary):  displayBase = DisplayBase::Binary;  break;
    default:                                     displayBase = DisplayBase::Hex;     break;
    }
    const char* actionName = displayBase == DisplayBase::Decimal ? "baseDecimal"
                           : displayBase == DisplayBase::Binary  ? "baseBinary"
                                                                 : "baseHex";
    if (QAction* a = findChild<QAction*>(actionName))
        a->setChecked(true);
    memoryPanel->setDisplayBase(displayBase);
    registerPanel->setDisplayBase(displayBase);

    const QByteArray geometry = settings.value("geometry").toByteArray();
    if (!geometry.isEmpty())
        restoreGeometry(geometry);

    rebuildRecentMenu();
}

void MainWindow::saveSettings()
{
    QSettings settings(kOrganization, kApplication);
    settings.setValue("recentFiles", recentFiles);
    settings.setValue("darkTheme", ThemeManager::current() == Theme::Dark);
    settings.setValue("displayBase", static_cast<int>(displayBase));
    settings.setValue("geometry", saveGeometry());
}

void MainWindow::addRecentFile(const QString& path)
{
    const QString absolute = QFileInfo(path).absoluteFilePath();
    recentFiles.removeAll(absolute);
    recentFiles.prepend(absolute);
    while (recentFiles.size() > kMaxRecentFiles)
        recentFiles.removeLast();
    rebuildRecentMenu();
}

void MainWindow::rebuildRecentMenu()
{
    if (!recentMenu)
        return;
    recentMenu->clear();

    if (recentFiles.isEmpty()) {
        QAction* none = recentMenu->addAction("(empty)");
        none->setEnabled(false);
        return;
    }

    for (const QString& path : recentFiles) {
        QAction* a = recentMenu->addAction(QFileInfo(path).fileName());
        a->setData(path);
        a->setToolTip(path);
        a->setStatusTip(path);
        connect(a, &QAction::triggered, this, &MainWindow::onOpenRecent);
    }
    recentMenu->addSeparator();
    QAction* clear = recentMenu->addAction("Clear list");
    connect(clear, &QAction::triggered, this, [this]() {
        recentFiles.clear();
        rebuildRecentMenu();
    });
}

// ============================================================================
// loading
// ============================================================================

bool MainWindow::openProgram(const QString& path)
{
    int start = 0;
    if (const std::optional<int> typed = controlBar->startAddress())
        start = *typed;

    if (!Memory::isEvenAddress(start) || start >= Memory::screenAddress) {
        showMessage(StatusText::invalidInput(
            "The start address must be even and below FF (for example 00 or 10)."));
        return false;
    }

    if (QFileInfo(path).absoluteFilePath() != QFileInfo(currentFilePath).absoluteFilePath())
        breakpoints.clear();

    if (!loadFile(path, start))
        return false;

    controlBar->setStartAddress(start);
    addRecentFile(path);
    return true;
}

bool MainWindow::loadFile(const QString& path, int startAddress)
{
    stopRunning();
    machine.reset();
    skipBreakpointOnce = false;
    stepCount = 0;

    const LoadResult result = machine.loadProgram(path.toStdString(), startAddress);
    const StatusMessage message = StatusText::fromLoad(result);

    if (!result.success) {
        // A failed load can leave partial data behind: start from a clean machine.
        machine.reset();
        programLoaded = false;
        loadedWords = 0;
        uiState = UiState::NoProgram;
        before = MachineSnapshot::take(machine);
        refresh();
        showMessage(message);
        return false;
    }

    // Make sure execution starts at the load address.
    machine.setProgramCounter(static_cast<uint8_t>(startAddress));

    programLoaded = true;
    currentFilePath = path;
    loadedStart = startAddress;
    loadedWords = result.wordsLoaded;
    uiState = UiState::Ready;

    before = MachineSnapshot::take(machine);   // nothing is highlighted right after a load
    refresh();
    showMessage(message);
    return true;
}

void MainWindow::onOpen()
{
    QString startDir;
    if (!currentFilePath.isEmpty())
        startDir = QFileInfo(currentFilePath).absolutePath();
    else if (!recentFiles.isEmpty())
        startDir = QFileInfo(recentFiles.first()).absolutePath();

    const QString path = QFileDialog::getOpenFileName(
        this, "Open Vole program", startDir,
        "Vole programs (*.txt *.vole *.hex);;All files (*)");
    if (path.isEmpty())
        return;
    openProgram(path);
}

void MainWindow::onOpenRecent()
{
    auto* action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    const QString path = action->data().toString();
    if (path.isEmpty())
        return;

    if (!QFileInfo::exists(path)) {
        recentFiles.removeAll(path);
        rebuildRecentMenu();
        showMessage(StatusText::invalidInput(QString("File not found: %1").arg(path)));
        return;
    }
    openProgram(path);
}

void MainWindow::onRestart()
{
    if (currentFilePath.isEmpty())
        return;
    loadFile(currentFilePath, loadedStart);    // breakpoints are kept
}

void MainWindow::onReset()
{
    stopRunning();
    machine.reset();

    programLoaded = false;
    loadedWords = 0;
    loadedStart = 0;
    currentFilePath.clear();
    stepCount = 0;
    breakpoints.clear();
    skipBreakpointOnce = false;
    uiState = UiState::NoProgram;

    before = MachineSnapshot::take(machine);
    refresh();
    showMessage(StatusText::invalidInput("Machine reset."));
}

// ============================================================================
// execution
// ============================================================================

void MainWindow::executeOneStep()
{
    before = MachineSnapshot::take(machine);
    const uint8_t pcBefore = machine.getCpu().getProgramCounter();

    const StepStatus status = machine.step();

    if (stepCount >= 0 && (status == StepStatus::OK ||
                           (status == StepStatus::HALTED && !before.halted)))
        ++stepCount;

    if (status != StepStatus::OK) {
        stopRunning();
        uiState = UiState::Halted;           // sticky until reload / reset / PC change
        refresh();
        showMessage(StatusText::fromStep(status, pcBefore));
        return;
    }
    refresh();
}

void MainWindow::onStep()
{
    if (!programLoaded)
        return;
    executeOneStep();
}

void MainWindow::onRunAll()
{
    if (!programLoaded)
        return;

    stopRunning();
    before = MachineSnapshot::take(machine);
    const uint8_t pcBefore = machine.getCpu().getProgramCounter();

    const StepStatus status = machine.runAll();

    stepCount = -1;                           // runAll does not report how many steps it took

    if (status == StepStatus::HALTED ||
        status == StepStatus::INVALID_INSTRUCTION ||
        status == StepStatus::INVALID_PROGRAM_COUNTER)
        uiState = UiState::Halted;

    refresh();
    showMessage(StatusText::fromStep(status, pcBefore));
}

void MainWindow::onRunToggled(bool running)
{
    if (!running) {
        stopRunning();
        return;
    }

    if (!programLoaded || machine.getCpu().isHalted() || uiState == UiState::Halted) {
        updateUiState();
        return;
    }

    skipBreakpointOnce = hitBreakpoint();     // lets Run continue from a breakpoint
    runTimer.setInterval(controlBar->stepDelayMs());
    runTimer.start();
    updateUiState();
    updateStatusBar();
}

void MainWindow::onTimerTick()
{
    if (stepCount >= 0 && stepCount >= maxAnimatedSteps) {
        stopRunning();
        showMessage(StatusText::fromStep(StepStatus::STEP_LIMIT_REACHED,
                                         machine.getCpu().getProgramCounter()));
        refresh();
        return;
    }

    if (hitBreakpoint()) {
        if (skipBreakpointOnce) {
            skipBreakpointOnce = false;
        } else {
            stopRunning();
            StatusMessage m = StatusText::invalidInput(
                QString("Breakpoint at %1.").arg(GuiUtil::hexByte(machine.getCpu().getProgramCounter())));
            m.severity = MessageSeverity::Info;
            refresh();
            showMessage(m);
            return;
        }
    } else {
        skipBreakpointOnce = false;
    }

    executeOneStep();
}

void MainWindow::onSpeedChanged(int delayMs)
{
    if (runTimer.isActive())
        runTimer.setInterval(delayMs);
}

void MainWindow::stopRunning()
{
    runTimer.stop();
    updateUiState();
}

bool MainWindow::hitBreakpoint() const
{
    return breakpoints.count(machine.getCpu().getProgramCounter()) != 0;
}

// ============================================================================
// edits coming from the panels
// ============================================================================

void MainWindow::onMemoryEdited(uint8_t address, uint8_t value)
{
    if (uiState == UiState::Running) {
        refresh();
        return;
    }
    if (address == Memory::screenAddress) {
        onEditRejected("Cell FF is the screen and cannot be edited.");
        refresh();
        return;
    }

    before = MachineSnapshot::take(machine);   // so the edited cell is highlighted
    machine.writeMemory(address, value);
    refresh();
}

void MainWindow::onRegisterEdited(uint8_t index, uint8_t value)
{
    if (uiState == UiState::Running) {
        refresh();
        return;
    }

    before = MachineSnapshot::take(machine);
    if (!machine.writeRegister(index, value)) {
        onEditRejected(QString("Could not write register R%1.")
                           .arg(QString::number(index, 16).toUpper()));
    }
    refresh();
}

void MainWindow::onProgramCounterEdited(uint8_t address)
{
    onSetProgramCounterRequested(address);
}

void MainWindow::onSetProgramCounterRequested(uint8_t address)
{
    if (uiState == UiState::Running) {
        refresh();
        return;
    }

    before = MachineSnapshot::take(machine);
    if (!machine.setProgramCounter(address)) {
        onEditRejected(QString("Cannot set PC to %1: it must be an even address below FF.")
                           .arg(GuiUtil::hexByte(address)));
        refresh();
        return;
    }

    if (programLoaded)
        uiState = UiState::Ready;             // clears a sticky "halted by error" state
    refresh();
}

void MainWindow::onEditRejected(const QString& reason)
{
    showMessage(StatusText::invalidInput(reason));
}

void MainWindow::onBreakpointToggled(uint8_t address, bool enabled)
{
    if (enabled)
        breakpoints.insert(address);
    else
        breakpoints.erase(address);
}

// ============================================================================
// view / help
// ============================================================================

void MainWindow::onDisplayBaseSelected(QAction* action)
{
    if (!action)
        return;

    switch (action->data().toInt()) {
    case static_cast<int>(DisplayBase::Decimal): displayBase = DisplayBase::Decimal; break;
    case static_cast<int>(DisplayBase::Binary):  displayBase = DisplayBase::Binary;  break;
    default:                                     displayBase = DisplayBase::Hex;     break;
    }

    memoryPanel->setDisplayBase(displayBase);
    registerPanel->setDisplayBase(displayBase);
    refresh();
}

void MainWindow::onThemeToggled(bool dark)
{
    ThemeManager::apply(dark ? Theme::Dark : Theme::Light);
    refresh();                                 // repaint highlight colors
}

void MainWindow::onShowInstructionSet()
{
    InstructionSetDialog dialog(this);
    dialog.exec();
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "About Vole Machine Simulator",
        "<b>Vole Machine Simulator</b><br>"
        "A step-by-step simulator of the Vole machine language.<br><br>"
        "Shortcuts: F10 step, F5 run all, F6 run/pause, Ctrl+R reset.<br>"
        "Click the first column of the program list to set a breakpoint; "
        "double-click a row to move the PC there.");
}

// ============================================================================
// display
// ============================================================================

void MainWindow::refresh()
{
    const Cpu& cpu = machine.getCpu();
    const uint8_t pc = cpu.getProgramCounter();

    memoryPanel->showMemory(machine.getMemory(), pc, before);
    registerPanel->showRegisters(cpu, before);
    screenPanel->showOutput(machine.getScreen().getOutput());

    programList->setBreakpoints(breakpoints);
    if (programLoaded)
        programList->showProgram(machine, loadedStart, loadedWords, pc);
    else
        programList->clearProgram();

    updateUiState();
    updateStatusBar();
}

void MainWindow::updateUiState()
{
    if (!programLoaded)
        uiState = UiState::NoProgram;
    else if (runTimer.isActive())
        uiState = UiState::Running;
    else if (machine.getCpu().isHalted() || uiState == UiState::Halted)
        uiState = UiState::Halted;
    else
        uiState = UiState::Ready;

    controlBar->setUiState(uiState);

    const bool editable = (uiState != UiState::Running);
    memoryPanel->setEditingEnabled(editable);
    registerPanel->setEditingEnabled(editable);

    stepAction->setEnabled(uiState == UiState::Ready);
    runAllAction->setEnabled(uiState == UiState::Ready);
    runAction->setEnabled(uiState == UiState::Ready || uiState == UiState::Running);
    runAction->setText(uiState == UiState::Running ? "&Pause" : "&Run / Pause");
    restartAction->setEnabled(uiState == UiState::Ready || uiState == UiState::Halted);
    resetAction->setEnabled(true);
}

void MainWindow::updateStatusBar()
{
    QString state;
    switch (uiState) {
    case UiState::NoProgram: state = "No program"; break;
    case UiState::Ready:     state = "Ready";      break;
    case UiState::Running:   state = "Running";    break;
    case UiState::Halted:    state = "Halted";     break;
    }
    stateLabel->setText(QString("  %1  ").arg(state));
    pcLabel->setText(QString("  PC: %1  ")
                         .arg(GuiUtil::hexByte(machine.getCpu().getProgramCounter())));
    stepsLabel->setText(stepCount >= 0 ? QString("  Steps: %1  ").arg(stepCount)
                                       : QString("  Steps: n/a  "));

    if (programLoaded && !currentFilePath.isEmpty())
        setWindowTitle(QString("Vole Machine Simulator - %1").arg(QFileInfo(currentFilePath).fileName()));
    else
        setWindowTitle("Vole Machine Simulator");
}

void MainWindow::showMessage(const StatusMessage& message)
{
    // ---- colors by severity: edit these to taste ----
    QString style;
    switch (message.severity) {
    case MessageSeverity::Error:
        style = "QStatusBar { background-color: #C62828; color: white; font-weight: bold; }";
        break;
    case MessageSeverity::Warning:
        style = "QStatusBar { background-color: #F9A825; color: black; font-weight: bold; }";
        break;
    case MessageSeverity::Info:
        style = QString();   // default look
        break;
    }
    statusBar()->setStyleSheet(style);
    statusBar()->showMessage(message.text, 8000);

    if (!message.showDialog)
        return;

    switch (message.severity) {
    case MessageSeverity::Info:
        QMessageBox::information(this, "Vole", message.text);
        break;
    case MessageSeverity::Warning:
        QMessageBox::warning(this, "Vole", message.text);
        break;
    case MessageSeverity::Error:
        QMessageBox::critical(this, "Vole", message.text);
        break;
    }
}

// ============================================================================
// drag & drop, close
// ============================================================================

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    const QMimeData* mime = event->mimeData();
    if (mime && mime->hasUrls() && !mime->urls().isEmpty()
        && mime->urls().first().isLocalFile())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent* event)
{
    const QMimeData* mime = event->mimeData();
    if (!mime || !mime->hasUrls() || mime->urls().isEmpty())
        return;

    const QString path = mime->urls().first().toLocalFile();
    if (path.isEmpty())
        return;

    event->acceptProposedAction();
    openProgram(path);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    runTimer.stop();
    saveSettings();
    QMainWindow::closeEvent(event);
}
