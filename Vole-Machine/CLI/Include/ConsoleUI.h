#include <string>
#include "ConsoleView.h"


class Machine;


class ConsoleUI {
private:
    Machine& machine;
    ConsoleView view;

public:
    ConsoleUI(Machine& machine);
    void run();

private:
    void showMenu();
    void handleLoadProgram();
    void handleSetProgramCounter();
    void handleEditRegister();
    void handleEditMemory();
    void handleDisplayState();
    void handleExecute();
    void runAtOnce();
    void runStepByStep();
    void waitForEnter();
    bool askExitOrLoadAnother();
    int readHexValue(std::string prompt, int maxValue);
};
