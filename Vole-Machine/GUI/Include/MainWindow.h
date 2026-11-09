// MainWindow.h
#ifndef VOLE_GUI_MAINWINDOW_H
#define VOLE_GUI_MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <set>

#include "Machine.h"
#include "GuiTypes.h"
#include "MachineSnapshot.h"
#include "StatusText.h"
#include "ThemeManager.h"

class MemoryPanel;
class RegisterPanel;
class ScreenPanel;
class ProgramListPanel;
class ControlBar;
class QLabel;
class QMenu;
class QAction;
class QDragEnterEvent;
class QDropEvent;
class QCloseEvent;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

    // Used for command-line / "open with" support.
    bool openProgram(const QString& path);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private slots:
    // File / control bar
    void onOpen();
    void onOpenRecent();
    void onStep();
    void onRunAll();                       // machine.runAll(): all at once
    void onRunToggled(bool running);       // animated run, honours breakpoints
    void onTimerTick();
    void onRestart();                      // reload last file at same start address
    void onReset();                        // machine.reset()
    void onSpeedChanged(int delayMs);

    // Edits coming from the panels
    void onMemoryEdited(uint8_t address, uint8_t value);
    void onRegisterEdited(uint8_t index, uint8_t value);
    void onProgramCounterEdited(uint8_t address);
    void onSetProgramCounterRequested(uint8_t address);
    void onEditRejected(const QString& reason);
    void onBreakpointToggled(uint8_t address, bool enabled);

    // View / help
    void onDisplayBaseSelected(QAction* action);
    void onThemeToggled(bool dark);
    void onShowInstructionSet();
    void onAbout();

private:
    // ---- construction ----
    void buildUi();
    void buildMenus();
    void connectSignals();
    void loadSettings();     // recent files, theme, display base, window geometry
    void saveSettings();

    // ---- core actions ----
    bool loadFile(const QString& path, int startAddress);
    void executeOneStep();                 // snapshot + machine.step() + refresh
    void stopRunning();                    // stops the timer and updates the UI
    bool hitBreakpoint() const;            // is PC in the breakpoint set?

    // ---- display ----
    void refresh();                        // updates ALL panels from the machine
    void updateUiState();                  // computes UiState, enables/disables controls
    void updateStatusBar();                // PC, step count, state label
    void showMessage(const StatusMessage& message);
    void addRecentFile(const QString& path);
    void rebuildRecentMenu();

    // ---- model (a member: never copied) ----
    Machine machine;
    MachineSnapshot before;                // state before the last step

    // ---- program info ----
    QString currentFilePath;
    int loadedStart = 0;
    int loadedWords = 0;
    bool programLoaded = false;
    long long stepCount = 0;
    long long maxAnimatedSteps = 100000;  // same limit idea as runAll
    std::set<uint8_t> breakpoints;
    bool skipBreakpointOnce = false;       // lets Run continue from a breakpoint

    // ---- UI state ----
    UiState uiState = UiState::NoProgram;
    DisplayBase displayBase = DisplayBase::Hex;
    QTimer runTimer;
    QStringList recentFiles;

    // ---- widgets ----
    ControlBar*       controlBar = nullptr;
    MemoryPanel*      memoryPanel = nullptr;
    RegisterPanel*    registerPanel = nullptr;
    ScreenPanel*      screenPanel = nullptr;
    ProgramListPanel* programList = nullptr;

    QLabel* stateLabel = nullptr;          // Ready / Running / Halted
    QLabel* pcLabel = nullptr;
    QLabel* stepsLabel = nullptr;

    QMenu*   recentMenu = nullptr;
    QAction* stepAction = nullptr;         // F10
    QAction* runAllAction = nullptr;       // F5
    QAction* runAction = nullptr;          // F6
    QAction* resetAction = nullptr;        // Ctrl+R
    QAction* restartAction = nullptr;
    QAction* darkModeAction = nullptr;
};

#endif