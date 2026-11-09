// MachineSnapshot.cpp
#include "MachineSnapshot.h"
#include "Machine.h"

MachineSnapshot MachineSnapshot::take(const Machine& machine)
{
    MachineSnapshot s;

    const Memory& memory = machine.getMemory();
    for (std::size_t i = 0; i < s.memory.size(); ++i)
        s.memory[i] = memory.read(static_cast<uint8_t>(i));

    const Cpu& cpu = machine.getCpu();
    for (std::size_t i = 0; i < s.registers.size(); ++i)
        s.registers[i] = cpu.readRegister(static_cast<uint8_t>(i));

    s.programCounter      = cpu.getProgramCounter();
    s.instructionRegister = cpu.getInstructionRegister();
    s.halted              = cpu.isHalted();
    s.screenSize          = machine.getScreen().getOutput().size();
    return s;
}

bool MachineSnapshot::memoryChanged(const MachineSnapshot& newer, uint8_t address) const
{
    return memory[address] != newer.memory[address];
}

bool MachineSnapshot::registerChanged(const MachineSnapshot& newer, uint8_t index) const
{
    if (index >= registers.size())
        return false;
    return registers[index] != newer.registers[index];
}
