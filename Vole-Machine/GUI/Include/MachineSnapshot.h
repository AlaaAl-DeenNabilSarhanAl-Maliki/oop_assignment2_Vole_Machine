// MachineSnapshot.h
#ifndef VOLE_GUI_MACHINESNAPSHOT_H
#define VOLE_GUI_MACHINESNAPSHOT_H

#include <array>
#include <cstddef>
#include <cstdint>

class Machine;

// A copy of the visible machine state, taken BEFORE a step.
// After the step, panels compare the new state with this copy
// to highlight what changed. The model stays unaware of the GUI.
struct MachineSnapshot {
    std::array<uint8_t, 256> memory{};
    std::array<uint8_t, 16>  registers{};
    uint8_t     programCounter = 0;
    uint16_t    instructionRegister = 0;
    bool        halted = false;
    std::size_t screenSize = 0;      // number of bytes already on screen

    static MachineSnapshot take(const Machine& machine);

    bool memoryChanged(const MachineSnapshot& newer, uint8_t address) const;
    bool registerChanged(const MachineSnapshot& newer, uint8_t index) const;
};

#endif