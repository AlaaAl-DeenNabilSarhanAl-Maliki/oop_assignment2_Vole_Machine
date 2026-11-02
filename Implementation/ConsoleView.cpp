#include"Vole_Machine.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>

std::string ConsoleView::toHex(int value, int digits)
{
    std::stringstream ss;
    ss << std::uppercase << std::setfill('0') << std::setw(digits) << std::hex << value;
    return ss.str();
}

void ConsoleView::showMemory(const Memory& memory)
 {
    
    std::cout << "--- Memory Content ---\n";
    
    std::cout << "    "; 
    
    for (int col = 0; col < 16; ++col) 
    {
        std::cout << toHex(col, 2) << " ";
    }
    
    std::cout << "\n";

    for (int i = 0; i < Memory::Size; ++i) 
    {
        if (i % 16 == 0) 
        {
            std::cout << toHex(i, 2) << ": ";
        }

        std::cout << toHex(memory.read(i), 2) << " ";

        if ((i + 1) % 16 == 0)
        {
            std::cout << "\n";
        }
    }
    std::cout << "----------------------\n";
 }

void ConsoleView::showScreen(const Screen& screen)
{

    std::cout << "--- Screen Output ---\n";
    
    
     const auto& output = screen.getOutput();
    
    
    for (uint8_t val : output) 
    {
        if (val >= 32 && val <= 126) 
        {
            std::cout << static_cast<char>(val);
        } 
        
        else 
        {
            std::cout << ".";
        }
    }
}

void ConsoleView::showRegisters(const Cpu& cpu)
{
   std::cout << "--- CPU Registers ---\n";
    
    std::cout << "PC: " << toHex(cpu.getProgramCounter(), 2) << " | IR: " << toHex(cpu.getInstructionRegister(), 4) << "\n\n";
    
    for (uint8_t i = 0; i < 16; ++i) {
        std::cout << "R" << toHex(i, 1) << ": " << toHex(cpu.readRegister(i), 2) << "  ";
        if ((i + 1) % 4 == 0) {
            std::cout << "\n";
        }
    }
    std::cout << "---------------------\n";
}

void ConsoleView::showState(const Machine& machine)
{
    std::cout << "\n================ MACHINE STATE ================\n";
    
    showRegisters(machine.getCpu());
    
    showMemory(machine.getMemory());
    
    showScreen(machine.getScreen());
    
    std::cout << "===============================================\n\n";
}

void ConsoleView::showStatus(StepStatus status)
{
    std::cout << "[Step Status]: ";
    switch (status) {
        case StepStatus::OK:
            std::cout << "OK - Step executed successfully.\n";
            break;
        case StepStatus::HALTED:
            std::cout << "HALTED - Execution stopped (Halt instruction reached).\n";
            break;
        case StepStatus::INVALID_INSTRUCTION:
            std::cout << "ERROR - Invalid Instruction encountered!\n";
            break;
        case StepStatus::INVALID_PROGRAM_COUNTER:
            std::cout << "ERROR - Invalid Program Counter (Out of bounds)!\n";
            break;
        case StepStatus::STEP_LIMIT_REACHED:
            std::cout << "WARNING - Step limit reached.\n";
            break;
    }
}

void ConsoleView::showLoadResult(const LoadResult& result)
{
    std::cout << "\n[Load Result]: ";
    
    if (result.success) 
    {
        
        std::cout << "SUCCESS - Program loaded successfully.\n";
        std::cout << "Details: " << result.message << "\n";
        std::cout << "Words Loaded: " << result.wordsLoaded << "\n";
    } 
    
    else 
    {
        
        std::cout << "ERROR - Failed to load program.\n";
        std::cout << "Error Message: " << result.message << "\n";
        std::cout << "Error at Line Number: " << result.lineNumber << "\n";
    }
}
