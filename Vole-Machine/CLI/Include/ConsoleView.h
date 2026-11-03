#ifndef VOLE_CONSOLEVIEW_H
#define VOLE_CONSOLEVIEW_H

#include<string>
#include "Common.h"

class Machine;
class Memory;
class Cpu;
class Screen;

class ConsoleView {
public:
    void showState(const Machine& machine);
    void showMemory(const Memory& memory);
    void showRegisters(const Cpu& cpu);
    void showScreen(const Screen& screen);
    void showStatus(StepStatus status);
    void showLoadResult(const LoadResult& result);

private:
    std::string toHex(int value, int digits);
};

#endif