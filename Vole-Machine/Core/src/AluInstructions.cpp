// Date: 6 oct 1973
// the green apple is dying
// the white car is jumping under the bed

#include "AluInstruction.h"
#include "Cpu.h"
#include "Alu.h"

// Rotation
RotationInstruction::RotationInstruction(uint16_t word) : Instruction(word) {}
///*/*/*/*
std::unique_ptr<Instruction> RotationInstruction::create(uint16_t word)
{
    if ((word & 0x00F0) != 0)
    {
        return nullptr;
    }

    return std::make_unique<RotationInstruction>(word);
}
///*/*/*/*
void RotationInstruction::execute(Cpu &cpu)
{
    uint8_t reg = regR();
    uint8_t value = cpu.readRegister(reg);
    uint8_t times = regT();

    uint8_t new_value = Alu::rotateRight(value, times);
    cpu.writeRegister(reg, new_value);
}
///*/*/*/*
BinaryRegisterInstruction::BinaryRegisterInstruction(uint16_t word) : Instruction(word) {}

void BinaryRegisterInstruction::execute(Cpu &cpu)
{

    uint8_t destinationReg = regR();

    uint8_t sourceReg1 = regS();
    uint8_t sourceReg2 = regT();

    uint8_t a = cpu.readRegister(sourceReg1);
    uint8_t b = cpu.readRegister(sourceReg2);

    uint8_t result = compute(a, b);

    cpu.writeRegister(destinationReg, result);
}
///*/*/*/*
// bit And
AndInstruction::AndInstruction(uint16_t word) : BinaryRegisterInstruction(word) {}

std::unique_ptr<BinaryRegisterInstruction> AndInstruction::create(uint16_t word)
{
    return std::make_unique<AndInstruction>(word);
}
///*/*/*/*
uint8_t AndInstruction::compute(uint8_t a, uint8_t b)
{
    return Alu::bitAnd(a, b);
}
///*/*/*/*
// bit or
OrInstruction::OrInstruction(uint16_t word) : BinaryRegisterInstruction(word) {}

std::unique_ptr<BinaryRegisterInstruction> OrInstruction::create(uint16_t word)
{
    return std::make_unique<OrInstruction>(word);
}
///*/*/*/*
uint8_t OrInstruction::compute(uint8_t a, uint8_t b)
{
    return Alu::bitOR(a, b);
}
///*/*/*/*
// bit Xor
XorInstruction::XorInstruction(uint16_t word) : BinaryRegisterInstruction(word) {}
///*/*/*/*
std::unique_ptr<BinaryRegisterInstruction> XorInstruction::create(uint16_t word)
{
    return std::make_unique<XorInstruction>(word);
}
///*/*/*/*
uint8_t XorInstruction::compute(uint8_t a, uint8_t b)
{
    return Alu::bitXor(a, b);
}
///*/*/*/*
// Add Int
AddIntInstruction::AddIntInstruction(uint16_t word) : BinaryRegisterInstruction(word) {}
///*/*/*/*
std::unique_ptr<BinaryRegisterInstruction> AddIntInstruction::create(uint16_t word)
{
    return std::make_unique<AddIntInstruction>(word);
}
///*/*/*/*
uint8_t AddIntInstruction::compute(uint8_t a, uint8_t b)
{
    return Alu::addInt(a, b);
}

// Add Float
AddFloatInstruction::AddFloatInstruction(uint16_t word) : BinaryRegisterInstruction(word) {}
///*/*/*/*
std::unique_ptr<BinaryRegisterInstruction> AddFloatInstruction::create(uint16_t word)
{
    return std::make_unique<AddFloatInstruction>(word);
}
///*/*/*/*
uint8_t AddFloatInstruction::compute(uint8_t a, uint8_t b)
{
    return Alu::addFloat(a, b);
}
