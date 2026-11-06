#ifndef VOLE_MACHINE_CLASS_H
#define VOLE_MACHINE_CLASS_H

#include<cstdint>
#include<string>
#include "Common.h"
#include "Screen.h"
#include "Memory.h"
#include "InstructionDecoder.h"
#include "Cpu.h"
#include "ProgramLoader.h"

class Machine {
    private:
        Screen screen;
        Memory memory;
        InstructionDecoder decoder;
        Cpu cpu;
        ProgramLoader loader;
    
    public:
        Machine(const Machine&) = delete;
        Machine& operator=(const Machine&) = delete;
        Machine();
        LoadResult loadProgram(const std::string& , int startAddress);
        bool setProgramCounter(uint8_t address);
        bool writeRegister(uint8_t index, uint8_t value);
        void writeMemory(uint8_t address, uint8_t value);
        StepStatus step();
        StepStatus runAll(int maxSteps = 100000);
        void reset();
        const Memory& getMemory() const;
        const Screen& getScreen() const;
        const Cpu& getCpu() const;

};

#endif