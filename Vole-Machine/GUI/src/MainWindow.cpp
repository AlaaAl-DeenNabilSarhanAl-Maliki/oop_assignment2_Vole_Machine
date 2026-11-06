#include "MainWindow.h"

#include "ControlBar.h"
#include "InstructionSetDialog.h"
#include "Memory.h"
#include "MemoryPanel.h"
#include "ProgramListPanel.h"
#include "RegisterPanel.h"
#include "ScreenPanel.h"

#include <QAction>
#include <QActionGroup>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QGroupBox>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QSettings>
#include <QSplitter>
#include <QStatusBar>
#include <QUrl>
#include <QVBoxLayout>

namespace {

const int kMaxRecentFiles = 5;

QGroupBox* wrapInGroup(const QString& title, QWidget* inner)
{
    auto* box = new QGroupBox(title);
    auto* layout = new QVBoxLayout(box);
    layout->addWidget(inner);
    return box;
}

// The address highlighted in the memory table and in the program list.
// After HALT the PC points past the HALT instruction, so we highlight HALT itself.
uint8_t addressToHighlight(const Machine& machine)
{
    const Cpu& cpu = machine.getCpu();
    const uint8_t pc = cpu.getProgramCounter();

    if (cpu.isHalted() && pc >= 2) {
        const uint8_t opcode = machine.getMemory().read(static_cast<uint8_t>(pc - 2)) >> 4;
        if (opcode == 0xC) return static_cast<uint8_t>(pc - 2);
    }
    return pc;
}

} // namespace

/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Vole Machine Simulator");
    setAcceptDrops(true);
    resize(1180, 760);

    buildUi();
    buildMenus();
    connectSignals();
    loadSettings();

    before = MachineSnapshot::take(machine);
    runTimer.setInterval(controlBar->stepDelayMs());
    refresh();
}

/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MainWindow::buildUi()
{
    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);

    controlBar = new ControlBar(central);
    layout->addWidget(controlBar);

    memoryPanel   = new MemoryPanel();
    registerPanel = new RegisterPanel();
    screenPanel   = new ScreenPanel();
    programList   = new ProgramListPanel();

    auto* topSplit = new QSplitter(Qt::Horizontal);
    topSplit->addWidget(wrapInGroup("Memory (address = row + column)", memoryPanel));
    topSplit->addWidget(wrapInGroup("Registers", registerPanel));
    topSplit->setStretchFactor(0, 3);
    topSplit->setStretchFactor(1, 1);

    auto* bottomSplit = new QSplitter(Qt::Horizontal);
    bottomSplit->addWidget(wrapInGroup("Program", programList));
    bottomSplit->addWidget(wrapInGroup("Screen", screenPanel));
    bottomSplit->setStretchFactor(0, 1);
    bottomSplit->setStretchFactor(1, 1);

    auto* vertical = new QSplitter(Qt::Vertical);
    vertical->addWidget(topSplit);
    vertical->addWidget(bottomSplit);
    vertical->setStretchFactor(0, 3);
    vertical->setStretchFactor(1, 2);
    layout->addWidget(vertical, 1);

    setCentralWidget(central);

    stateLabel = new QLabel("No program loaded");
    pcLabel    = new QLabel("PC: 00");
    stepsLabel = new QLabel("Steps: 0");
    statusBar()->addPermanentWidget(stateLabel);
    statusBar()->addPermanentWidget(pcLabel);
    statusBar()->addPermanentWidget(stepsLabel);
}

void MainWindow::buildMenus()
{
    // ---------------- File ----------------
    QMenu* fileMenu = menuBar()->addMenu("&File");

    QAction* openAction = fileMenu->addAction("&Open program...");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::onOpen);

    recentMenu = fileMenu->addMenu("Open &recent");

    restartAction = fileMenu->addAction("Re&start program");
    restartAction->setShortcut(QKeySequence("Ctrl+Shift+R"));
    connect(restartAction, &QAction::triggered, this, &MainWindow::onRestart);

    fileMenu->addSeparator();
    QAction* exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // ---------------- Run ----------------
    QMenu* runMenu = menuBar()->addMenu("&Run");

    stepAction = runMenu->addAction("&Step");
    stepAction->setShortcut(QKeySequence(Qt::Key_F10));
    connect(stepAction, &QAction::triggered, this, &MainWindow::onStep);

    runAllAction = runMenu->addAction("Run &all at once");
    runAllAction->setShortcut(QKeySequence(Qt::Key_F5));
    connect(runAllAction, &QAction::triggered, this, &MainWindow::onRunAll);

    runAction = runMenu->addAction("Run / &Pause (animated)");
    runAction->setShortcut(QKeySequence(Qt::Key_F6));
    connect(runAction, &QAction::triggered, this, [this]() {
        onRunToggled(uiState != UiState::Running);
    });

    runMenu->addSeparator();
    resetAction = runMenu->addAction("&Reset machine");
    resetAction->setShortcut(QKeySequence("Ctrl+R"));
    connect(resetAction, &QAction::triggered, this, &MainWindow::onReset);

    // ---------------- View ----------------
    QMenu* viewMenu = menuBar()->addMenu("&View");
    QMenu* baseMenu = viewMenu->addMenu("Number &base");
    auto* baseGroup = new QActionGroup(this);
    baseGroup->setExclusive(true);

    const char* baseNames[] = {"Hexadecimal", "Decimal", "Binary"};
    for (int i = 0; i < 3; ++i) {
        QAction* action = baseMenu->addAction(baseNames[i]);
        action->setCheckable(true);
        action->setData(i);
        action->setObjectName(QString("baseAction%1").arg(i));
        baseGroup->addAction(action);
        if (i == 0) action->setChecked(true);
    }
    connect(baseGroup, &QActionGroup::triggered, this, &MainWindow::onDisplayBaseSelected);

    darkModeAction = viewMenu->addAction("&Dark mode");
    darkModeAction->setCheckable(true);
    connect(darkModeAction, &QAction::toggled, this, &MainWindow::onThemeToggled);

    // ---------------- Help ----------------
    QMenu* helpMenu = menuBar()->addMenu("&Help");
    QAction* setAction = helpMenu->addAction("&Instruction set");
    setAction->setShortcut(QKeySequence(Qt::Key_F1));
    connect(setAction, &QAction::triggered, this, &MainWindow::onShowInstructionSet);
    QAction* aboutAction = helpMenu->addAction("&About");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
}

void MainWindow::connectSignals()
{
    connect(controlBar, &ControlBar::loadClicked,    this, &MainWindow::onOpen);
    connect(controlBar, &ControlBar::stepClicked,    this, &MainWindow::onStep);
    connect(controlBar, &ControlBar::runAllClicked,  this, &MainWindow::onRunAll);
    connect(controlBar, &ControlBar::runToggled,     this, &MainWindow::onRunToggled);
    connect(controlBar, &ControlBar::restartClicked, this, &MainWindow::onRestart);
    connect(controlBar, &ControlBar::resetClicked,   this, &MainWindow::onReset);
    connect(controlBar, &ControlBar::speedChanged,   this, &MainWindow::onSpeedChanged);

    connect(memoryPanel, &MemoryPanel::memoryEdited, this, &MainWindow::onMemoryEdited);
    connect(memoryPanel, &MemoryPanel::editRejected, this, &MainWindow::onEditRejected);
    connect(memoryPanel, &MemoryPanel::setProgramCounterRequested,
            this, &MainWindow::onSetProgramCounterRequested);

    connect(registerPanel, &RegisterPanel::registerEdited, this, &MainWindow::onRegisterEdited);
    connect(registerPanel, &RegisterPanel::programCounterEdited,
            this, &MainWindow::onProgramCounterEdited);
    connect(registerPanel, &RegisterPanel::editRejected, this, &MainWindow::onEditRejected);

    connect(programList, &ProgramListPanel::breakpointToggled,
            this, &MainWindow::onBreakpointToggled);
    connect(programList, &ProgramListPanel::setProgramCounterRequested,
            this, &MainWindow::onSetProgramCounterRequested);

    connect(&runTimer, &QTimer::timeout, this, &MainWindow::onTimerTick);
}

/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MainWindow::loadSettings()
{
    QSettings settings("FCAI-CU", "VoleSimulator");

    restoreGeometry(settings.value("geometry").toByteArray());
    recentFiles = settings.value("recentFiles").toStringList();
    rebuildRecentMenu();

    const int baseIndex = settings.value("displayBase", 0).toInt();
    displayBase = static_cast<DisplayBase>(baseIndex >= 0 && baseIndex <= 2 ? baseIndex : 0);
    if (QAction* baseAction = findChild<QAction*>(QString("baseAction%1").arg(static_cast<int>(displayBase))))
        baseAction->setChecked(true);
    memoryPanel->setDisplayBase(displayBase);
    registerPanel->setDisplayBase(displayBase);

    const bool dark = settings.value("darkMode", false).toBool();
    ThemeManager::apply(dark ? Theme::Dark : Theme::Light);
    {
        QSignalBlocker blocker(darkModeAction);
        darkModeAction->setChecked(dark);
    }

    const int start = settings.value("startAddress", 0).toInt();
    controlBar->setStartAddress(start);
}

void MainWindow::saveSettings()
{
    QSettings settings("FCAI-CU", "VoleSimulator");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("recentFiles", recentFiles);
    settings.setValue("displayBase", static_cast<int>(displayBase));
    settings.setValue("darkMode", ThemeManager::current() == Theme::Dark);
    settings.setValue("startAddress", controlBar->startAddress().value_or(0));
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    runTimer.stop();
    saveSettings();
    event->accept();
}

/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent* event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty() || !urls.first().isLocalFile()) return;
    openProgram(urls.first().toLocalFile());
}

/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MainWindow::onOpen()
{
    const QString folder = currentFilePath.isEmpty()
                               ? QString()
                               : QFileInfo(currentFilePath).absolutePath();

    const QString path = QFileDialog::getOpenFileName(
        this, "Open Vole program", folder,
        "Vole programs (*.txt *.vole);;All files (*)");

    if (!path.isEmpty()) openProgram(path);
}

void MainWindow::onOpenRecent()
{
    const QAction* action = qobject_cast<QAction*>(sender());
    if (action != nullptr) openProgram(action->data().toString());
}

bool MainWindow::openProgram(const QString& path)
{
    const std::optional<int> start = controlBar->startAddress();

    if (!start.has_value()) {
        StatusMessage message = StatusText::invalidInput(
            "Start address must be a hex value between 00 and FF.");
        message.showDialog = true;
        showMessage(message);
        return false;
    }
    if (!Memory::isEvenAddress(*start) || *start >= Memory::screenAddress) {
        StatusMessage message = StatusText::invalidInput(
            "Start address must be an even number lower than FF.");
        message.showDialog = true;
        showMessage(message);
        return false;
    }

    return loadFile(path, *start);
}

bool MainWindow::loadFile(const QString& path, int startAddress)
{
    stopRunning();

    // toLocal8Bit: the model opens the file with std::ifstream (narrow path).
    const LoadResult result = machine.loadProgram(path.toLocal8Bit().toStdString(), startAddress);
    showMessage(StatusText::fromLoad(result));

    if (!result.success) {
        updateUiState();   // a failed load changes nothing in the machine
        return false;
    }

    if (path != currentFilePath) {
        breakpoints.clear();
        programList->setBreakpoints(breakpoints);
    }

    currentFilePath = path;
    loadedStart = startAddress;
    loadedWords = result.wordsLoaded;
    programLoaded = true;
    stepCount = 0;
    skipBreakpointOnce = false;

    before = MachineSnapshot::take(machine);
    addRecentFile(path);
    setWindowTitle(QFileInfo(path).fileName() + " - Vole Machine Simulator");
    refresh();
    return true;
}

/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MainWindow::onStep()
{
    if (uiState != UiState::Ready) return;
    executeOneStep();
}

void MainWindow::executeOneStep()
{
    before = MachineSnapshot::take(machine);
    const StepStatus status = machine.step();

    // A step counts when an instruction really ran (HALT counts too).
    if (status == StepStatus::OK || (status == StepStatus::HALTED && !before.halted)) {
        if (stepCount < 0) stepCount = 0;
        ++stepCount;
    }

    // Stop the timer BEFORE any dialog opens (a dialog keeps the event loop alive).
    if (status != StepStatus::OK) runTimer.stop();

    refresh();

    if (status != StepStatus::OK) {
        StatusMessage message = StatusText::fromStep(status, machine.getCpu().getProgramCounter());
        if (status == StepStatus::INVALID_INSTRUCTION) {
            message.text += ". Fix the memory, then right-click a cell > \"Set PC here\" to continue.";
        }
        showMessage(message);
    }
}

void MainWindow::onRunAll()
{
    if (uiState != UiState::Ready) return;

    before = MachineSnapshot::take(machine);
    const StepStatus status = machine.runAll();
    stepCount = -1;   // runAll does not report how many steps it executed

    refresh();
    showMessage(StatusText::fromStep(status, machine.getCpu().getProgramCounter()));
}

void MainWindow::onRunToggled(bool running)
{
    if (!running) {
        stopRunning();
        return;
    }

    if (uiState != UiState::Ready) {
        updateUiState();   // put the Run button back in sync
        return;
    }

    if (stepCount < 0) stepCount = 0;
    runStartStep = stepCount;

    // If we start on a breakpoint, step off it first instead of stopping at once.
    skipBreakpointOnce = hitBreakpoint();

    runTimer.setInterval(controlBar->stepDelayMs());
    runTimer.start();
    updateUiState();
}

void MainWindow::onTimerTick()
{
    if (hitBreakpoint() && !skipBreakpointOnce) {
        stopRunning();
        statusBar()->showMessage(
            "Stopped at breakpoint " + GuiUtil::hexByte(machine.getCpu().getProgramCounter()), 5000);
        return;
    }
    skipBreakpointOnce = false;

    if (stepCount - runStartStep >= maxAnimatedSteps) {
        stopRunning();
        showMessage(StatusText::fromStep(StepStatus::STEP_LIMIT_REACHED,
                                         machine.getCpu().getProgramCounter()));
        return;
    }

    executeOneStep();
}

void MainWindow::stopRunning()
{
    runTimer.stop();
    skipBreakpointOnce = false;
    updateUiState();
}

bool MainWindow::hitBreakpoint() const
{
    return breakpoints.count(machine.getCpu().getProgramCounter()) > 0;
}

void MainWindow::onRestart()
{
    if (currentFilePath.isEmpty()) return;
    stopRunning();
    loadFile(currentFilePath, loadedStart);
}

void MainWindow::onReset()
{
    stopRunning();
    machine.reset();

    programLoaded = false;
    loadedWords = 0;
    stepCount = 0;
    currentFilePath.clear();
    breakpoints.clear();
    programList->setBreakpoints(breakpoints);

    before = MachineSnapshot::take(machine);
    setWindowTitle("Vole Machine Simulator");
    refresh();
    statusBar()->showMessage("Machine reset", 4000);
}

void MainWindow::onSpeedChanged(int delayMs)
{
    runTimer.setInterval(delayMs);
}

/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MainWindow::onMemoryEdited(uint8_t address, uint8_t value)
{
    before = MachineSnapshot::take(machine);   // so the edited cell is highlighted
    machine.writeMemory(address, value);
    refresh();
}

void MainWindow::onRegisterEdited(uint8_t index, uint8_t value)
{
    before = MachineSnapshot::take(machine);
    if (!machine.writeRegister(index, value)) {
        showMessage(StatusText::invalidInput("Invalid register number."));
    }
    refresh();
}

void MainWindow::onProgramCounterEdited(uint8_t address)
{
    onSetProgramCounterRequested(address);
}

void MainWindow::onSetProgramCounterRequested(uint8_t address)
{
    before = MachineSnapshot::take(machine);
    if (!machine.setProgramCounter(address)) {
        showMessage(StatusText::invalidInput("The program counter must be an even address."));
    }
    refresh();
}

void MainWindow::onEditRejected(const QString& reason)
{
    showMessage(StatusText::invalidInput(reason));
}

void MainWindow::onBreakpointToggled(uint8_t address, bool enabled)
{
    if (enabled) breakpoints.insert(address);
    else         breakpoints.erase(address);

    statusBar()->showMessage(QString("Breakpoint %1 at %2")
                                 .arg(enabled ? "set" : "removed")
                                 .arg(GuiUtil::hexByte(address)), 3000);
}

/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MainWindow::onDisplayBaseSelected(QAction* action)
{
    displayBase = static_cast<DisplayBase>(action->data().toInt());
    memoryPanel->setDisplayBase(displayBase);
    registerPanel->setDisplayBase(displayBase);
    refresh();
}

void MainWindow::onThemeToggled(bool dark)
{
    ThemeManager::apply(dark ? Theme::Dark : Theme::Light);
    refresh();   // highlight colors depend on the theme
}

void MainWindow::onShowInstructionSet()
{
    InstructionSetDialog dialog(this);
    dialog.exec();
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "About",
        "Vole Machine Simulator\n"
        "CS213 - Assignment 2\n"
        "Faculty of Computers and Artificial Intelligence, Cairo University");
}

/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MainWindow::refresh()
{
    const Cpu& cpu = machine.getCpu();
    const uint8_t highlight = addressToHighlight(machine);

    memoryPanel->showMemory(machine.getMemory(), highlight, before);
    registerPanel->showRegisters(cpu, before);
    screenPanel->showOutput(machine.getScreen().getOutput());

    if (programLoaded) programList->showProgram(machine, loadedStart, loadedWords, highlight);
    else               programList->clearProgram();

    updateUiState();
}

void MainWindow::updateUiState()
{
    if (runTimer.isActive())                    uiState = UiState::Running;
    else if (!programLoaded)                    uiState = UiState::NoProgram;
    else if (machine.getCpu().isHalted())       uiState = UiState::Halted;
    else                                        uiState = UiState::Ready;

    controlBar->setUiState(uiState);

    const bool editable = (uiState != UiState::Running);
    memoryPanel->setEditingEnabled(editable);
    registerPanel->setEditingEnabled(editable);

    stepAction->setEnabled(uiState == UiState::Ready);
    runAllAction->setEnabled(uiState == UiState::Ready);
    runAction->setEnabled(uiState == UiState::Ready || uiState == UiState::Running);
    restartAction->setEnabled(programLoaded && uiState != UiState::Running);
    resetAction->setEnabled(true);

    updateStatusBar();
}

void MainWindow::updateStatusBar()
{
    switch (uiState) {
    case UiState::NoProgram: stateLabel->setText("No program loaded"); break;
    case UiState::Ready:     stateLabel->setText("Ready");             break;
    case UiState::Running:   stateLabel->setText("Running...");        break;
    case UiState::Halted:    stateLabel->setText("Halted");            break;
    }

    pcLabel->setText("PC: " + GuiUtil::hexByte(machine.getCpu().getProgramCounter()));
    stepsLabel->setText(stepCount >= 0 ? QString("Steps: %1").arg(stepCount)
                                       : QString("Steps: -"));
}

void MainWindow::showMessage(const StatusMessage& message)
{
    statusBar()->showMessage(message.text, 8000);

    if (!message.showDialog) return;

    switch (message.severity) {
    case MessageSeverity::Error:
        QMessageBox::critical(this, "Vole Simulator", message.text);
        break;
    case MessageSeverity::Warning:
        QMessageBox::warning(this, "Vole Simulator", message.text);
        break;
    case MessageSeverity::Info:
        QMessageBox::information(this, "Vole Simulator", message.text);
        break;
    }
}

/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/

void MainWindow::addRecentFile(const QString& path)
{
    recentFiles.removeAll(path);
    recentFiles.prepend(path);
    while (recentFiles.size() > kMaxRecentFiles) recentFiles.removeLast();
    rebuildRecentMenu();
}

void MainWindow::rebuildRecentMenu()
{
    recentMenu->clear();

    for (const QString& path : recentFiles) {
        QAction* action = recentMenu->addAction(QFileInfo(path).fileName());
        action->setData(path);
        action->setToolTip(path);
        connect(action, &QAction::triggered, this, &MainWindow::onOpenRecent);
    }
    recentMenu->setEnabled(!recentFiles.isEmpty());
}
