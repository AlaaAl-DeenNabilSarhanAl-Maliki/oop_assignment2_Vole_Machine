#ifndef VOLE_CONTROLINSTRUCTIONS_H
#define VOLE_CONTROLINSTRUCTIONS_H

#include<cstdint>
#include<memory>
#include "Instruction.h"

class JumpInstruction:public Instruction {
    public:
        explicit JumpInstruction(uint16_t word);
        static std::unique_ptr<Instruction> create (uint16_t word);
        void execute(Cpu& cpu) override;
};

class HaltInstruction:public Instruction {
    public:
        explicit HaltInstruction(uint16_t word);
        static std::unique_ptr<Instruction> create (uint16_t word);
        void execute(Cpu& cpu) override;
};

#endif