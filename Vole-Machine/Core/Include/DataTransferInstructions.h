#ifndef VOLE_DATATRANSFERINSTRUCTIONS_H
#define VOLE_DATATRANSFERINSTRUCTIONS_H

#include<cstdint>
#include<memory>
#include "Instruction.h"

class LoadFromMemoryInstruction: public Instruction {
    public:
        explicit LoadFromMemoryInstruction(uint16_t word);
        static std::unique_ptr<Instruction> create (uint16_t word);
        void execute(Cpu& cpu) override;
};

class LoadImmediateInstruction:public Instruction {
    public:
        explicit LoadImmediateInstruction(uint16_t word);
        static std::unique_ptr<Instruction> create (uint16_t word);
        void execute(Cpu& cpu) override;
};

class StoreInstruction: public Instruction {
    public:
        explicit StoreInstruction(uint16_t word);
        static std::unique_ptr<Instruction> create (uint16_t word);
        void execute(Cpu& cpu) override;
};

class MoveInstruction:public Instruction {
    public:
        explicit MoveInstruction(uint16_t word);
    
        static std::unique_ptr<Instruction> create (uint16_t word);
        void execute(Cpu& cpu) override;
};

#endif