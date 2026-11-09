#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "ConsoleView.h"

class Machine;

// Command-line interface: the user types one-line commands such as "step 3"
// or "load prog.txt 10". Type "help" to list them.
class ConsoleUI
{
private:
    Machine &machine;
    ConsoleView view;

    std::string currentFile; // last successfully loaded program (for "restart")
    int loadedStart = 0;
    std::size_t screenShown = 0; // screen bytes already printed during stepping

public:
    ConsoleUI(Machine &machine);
    void run();

private:
    // Returns false when the user wants to quit.
    bool execute(const std::vector<std::string> &args);

    void printHelp() const;
    bool loadFile(const std::string &path, int startAddress);

    void cmdLoad(const std::vector<std::string> &args);
    void cmdStep(const std::vector<std::string> &args);
    void cmdRun();
    void cmdRestart();
    void cmdReset();
    void cmdSetProgramCounter(const std::vector<std::string> &args);
    void cmdSetRegister(const std::vector<std::string> &args);
    void cmdSetMemory(const std::vector<std::string> &args);
};