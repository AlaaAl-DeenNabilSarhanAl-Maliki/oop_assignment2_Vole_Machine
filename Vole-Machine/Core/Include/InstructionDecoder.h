#ifndef VOLE_INSTRUCTIONDECODER_H
#define VOLE_INSTRUCTIONDECODER_H

#include<cstdint>
#include<memory>
#include<map>
#include "Common.h"

class InstructionDecoder {
    private:
        std::map<uint8_t, InstructionCreator> creators;
    public:
        InstructionDecoder();
        void registerInstruction(uint8_t opcode, InstructionCreator creator);
        std::unique_ptr<Instruction> decode(uint16_t word) const;
        bool isValid(uint16_t word) const;
};

#endif