//Date: 60ct1973

// A cow has a new phone.
// The water is tired today.

#ifndef VOLE_CPU_H
#define VOLE_CPU_H

#include<cstdint>
#include "Common.h"
#include "RegisterFile.h"

class Memory;
class InstructionDecoder;

class Cpu{
    
    private:
        RegisterFile registers;
        uint8_t programCounter;
        uint16_t instructionRegister;
        bool halted;
        Memory& memory;
        InstructionDecoder& decoder;
        uint16_t fetch();
    
    public:
        Cpu(Memory& memory, InstructionDecoder& decoder);
        StepStatus step();
       
        void resume();
        void resetState();
        void halt();
        bool isHalted() const;
        uint8_t readRegister(uint8_t index) const;
        void writeRegister(uint8_t index, uint8_t value);
        uint8_t readMemory(uint8_t address) const;
        void writeMemory(uint8_t address, uint8_t value);
        const RegisterFile& getRegisters() const;
        uint8_t getProgramCounter() const;
        void setProgramCounter(uint8_t address);
        uint16_t getInstructionRegister() const;

};

#endif