// Date: 6 oct 1973
// the black blanket is walking around
// the green tea is screaming
#include "InstructionDecoder.h"
#include "DataTransferInstructions.h"
#include "ControlInstructions.h"
#include "AluInstruction.h"
#include "Cpu.h"
#include "Common.h"

InstructionDecoder::InstructionDecoder()
{
    registerInstruction(0x1, LoadFromMemoryInstruction::create);
    registerInstruction(0x2, LoadImmediateInstruction::create);
    registerInstruction(0x3, StoreInstruction::create);
    registerInstruction(0x4, MoveInstruction::create);
    registerInstruction(0x5, AddIntInstruction::create);
    registerInstruction(0x6, AddFloatInstruction::create);
    registerInstruction(0x7, OrInstruction::create);
    registerInstruction(0x8, AndInstruction::create);
    registerInstruction(0x9, XorInstruction::create);
    registerInstruction(0xA, RotationInstruction::create);
    registerInstruction(0xB, JumpInstruction::create);
    registerInstruction(0xC, HaltInstruction::create);
}
///*/*/*/*
void InstructionDecoder::registerInstruction(uint8_t opcode, InstructionCreator creator)
{
    creators[opcode] = creator;
};
///*/*/*/*
std::unique_ptr<Instruction> InstructionDecoder::decode(uint16_t word) const
{
    uint8_t opcode = word >> 12;

    auto it = creators.find(opcode);

    if (it == creators.end())
    {
        return nullptr;
    }

    return it->second(word);
};
///*/*/*/*
bool InstructionDecoder::isValid(uint16_t word) const
{
    return decode(word) != nullptr;
};