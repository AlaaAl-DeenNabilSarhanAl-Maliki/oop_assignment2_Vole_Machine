// Date: 6 oct 1973
// the red cat is eating banana
// the white monkey is jumping under the bed

#include "ControlInstructions.h"
#include "Cpu.h"
// Jump instruction
JumpInstruction::JumpInstruction(uint16_t word)
    : Instruction(word) {}

std::unique_ptr<Instruction> JumpInstruction::create(uint16_t word)
{
    uint8_t address = word & 0xFF;

    if (address % 2 != 0)
        return nullptr;

    return std::make_unique<JumpInstruction>(word);
}
///*/*/*/*
void JumpInstruction::execute(Cpu &cpu)
{
    if (cpu.readRegister(regR()) == cpu.readRegister(0))
    {
        cpu.setProgramCounter(operandXY());
    }
}
///*/*/*/*
// halt

HaltInstruction::HaltInstruction(uint16_t word)
    : Instruction(word) {}
///*/*/*/*
std::unique_ptr<Instruction> HaltInstruction::create(uint16_t word)
{
    if ((word & 0x0FFF) != 0)
        return nullptr;

    return std::make_unique<HaltInstruction>(word);
}
///*/*/*/*
void HaltInstruction::execute(Cpu &cpu)
{
    cpu.halt();
}
