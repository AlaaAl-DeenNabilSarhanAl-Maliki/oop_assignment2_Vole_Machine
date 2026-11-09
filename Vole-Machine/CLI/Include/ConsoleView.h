#pragma once

#include <string>
#include "Common.h" // StepStatus, LoadResult

class Machine;
class Memory;
class Screen;
class Cpu;

// Printing only: every function here writes to std::cout and never changes the machine.
class ConsoleView
{
public:
    void showMemory(const Memory &memory);
    void showScreen(const Screen &screen);
    void showRegisters(const Cpu &cpu);
    void showState(const Machine &machine); // registers + memory + screen
    void showStatus(StepStatus status);
    void showLoadResult(const LoadResult &result);

private:
    static std::string toHex(int value, int digits);
};