// Date: 6 oct 1973

// My table is eating a book.
// The sun has a small hat.

#include "Cpu.h"
#include "Memory.h"
#include "InstructionDecoder.h"

//*/*/*/*/*......
Cpu::Cpu(Memory &memory, InstructionDecoder &decoder) : programCounter(0),
                                                        instructionRegister(0),
                                                        halted(false),
                                                        memory(memory),
                                                        decoder(decoder)
{
}

//*/*/*/*/*......
uint8_t Cpu::getProgramCounter() const
{
    return programCounter;
}

//*/*/*/*/*......
void Cpu::setProgramCounter(uint8_t address)
{
    programCounter = address;
}

//*/*/*/*/*......
uint16_t Cpu::getInstructionRegister() const
{

    return instructionRegister;
}

//*/*/*/*/*......
const RegisterFile &Cpu::getRegisters() const
{

    return registers;
}

//*/*/*/*/*......
uint8_t Cpu::readRegister(uint8_t index) const
{

    return registers.get(index);
}

//*/*/*/*/*......
void Cpu::writeRegister(uint8_t index, uint8_t value)
{

    registers.set(index, value);
}

//*/*/*/*/*......
uint8_t Cpu::readMemory(uint8_t address) const
{

    return memory.read(address);
}

//*/*/*/*/*......
void Cpu::writeMemory(uint8_t address, uint8_t value)
{

    memory.write(address, value);
}

//*/*/*/*/*......
void Cpu::resetState()
{
    registers.clear();
    programCounter = 0;
    instructionRegister = 0;
    halted = false;
}

//*/*/*/*/*......
void Cpu::resume()
{
    halted = false;
}

//*/*/*/*/*......
static uint16_t combineBytes(uint8_t highByte, uint8_t lowByte)
{
    return (static_cast<uint16_t>(highByte) << 8) | lowByte;
}

//*/*/*/*/*......
uint16_t Cpu::fetch()
{
    uint8_t highByte = readMemory(programCounter);
    uint8_t lowByte = readMemory(programCounter + 1);

    instructionRegister = combineBytes(highByte, lowByte);

    programCounter += 2;
    return instructionRegister;
}

//*/*/*/*/*......
void Cpu::halt()
{
    halted = true;
}

//*/*/*/*/*......
bool Cpu::isHalted() const
{
    return halted;
}

//*/*/*/*/*......
StepStatus Cpu::step()
{
    if (isHalted())
    {
        return StepStatus::HALTED;
    }
    if (getProgramCounter() >= 254)
    {

        return StepStatus::INVALID_PROGRAM_COUNTER;
    }

    std::unique_ptr<Instruction> decodedInstruction = decoder.decode(fetch());

    if (decodedInstruction == nullptr)
    {
        halt();
        programCounter -= 2;
        return StepStatus::INVALID_INSTRUCTION;
    }

    decodedInstruction->execute(*this);

    if (isHalted())
    {
        return StepStatus::HALTED;
    }

    return StepStatus::OK;
}
