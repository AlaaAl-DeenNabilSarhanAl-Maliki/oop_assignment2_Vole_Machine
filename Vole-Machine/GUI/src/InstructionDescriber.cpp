// InstructionDescriber.cpp
#include "InstructionDescriber.h"

#include <iomanip>
#include <sstream>

// Word layout (4 hex digits):  [opcode][R][S][T]   with XY = last two digits.
//   nibbleR = 2nd digit, nibbleS = 3rd digit, nibbleT = 4th digit.

uint8_t InstructionDescriber::opcode(uint16_t word) { return static_cast<uint8_t>((word >> 12) & 0xF); }
uint8_t InstructionDescriber::nibbleR(uint16_t word) { return static_cast<uint8_t>((word >> 8) & 0xF); }
uint8_t InstructionDescriber::nibbleS(uint16_t word) { return static_cast<uint8_t>((word >> 4) & 0xF); }
uint8_t InstructionDescriber::nibbleT(uint16_t word) { return static_cast<uint8_t>(word & 0xF); }
uint8_t InstructionDescriber::operandXY(uint16_t word) { return static_cast<uint8_t>(word & 0xFF); }

std::string InstructionDescriber::hex(unsigned value, int digits)
{
    std::ostringstream out;
    out << std::uppercase << std::hex << std::setw(digits) << std::setfill('0') << value;
    return out.str();
}

std::string InstructionDescriber::reg(uint8_t index)
{
    return "R" + hex(index & 0xF, 1);
}

bool InstructionDescriber::isValidPattern(uint16_t word)
{
    switch (opcode(word))
    {
    case 0x1:
    case 0x2:
    case 0x3:
    case 0x5:
    case 0x6:
    case 0x7:
    case 0x8:
    case 0x9:
    case 0xB:
        return true;
    case 0x4: // 4 0 R S : second digit must be 0
        return nibbleR(word) == 0;
    case 0xA: // A R 0 X : third digit must be 0
        return nibbleS(word) == 0;
    case 0xC: // C 0 0 0
        return (word & 0x0FFF) == 0;
    default:
        return false;
    }
}

std::string InstructionDescriber::mnemonic(uint16_t word)
{
    if (!isValidPattern(word))
        return "???";
    switch (opcode(word))
    {
    case 0x1:
    case 0x2:
        return "LOAD";
    case 0x3:
        return "STORE";
    case 0x4:
        return "MOVE";
    case 0x5:
        return "ADD";
    case 0x6:
        return "FADD";
    case 0x7:
        return "OR";
    case 0x8:
        return "AND";
    case 0x9:
        return "XOR";
    case 0xA:
        return "ROTATE";
    case 0xB:
        return "JUMP";
    case 0xC:
        return "HALT";
    default:
        return "???";
    }
}

std::string InstructionDescriber::describe(uint16_t word)
{
    if (!isValidPattern(word))
    {
        const uint8_t op = opcode(word);
        // Opcodes 1..C with wrong "unused" digits are malformed instructions;
        // everything else is most likely plain data.
        if (op >= 0x1 && op <= 0xC)
            return "INVALID";
        return "DATA";
    }

    const std::string R = reg(nibbleR(word));
    const std::string S = reg(nibbleS(word));
    const std::string T = reg(nibbleT(word));
    const std::string XY = hex(operandXY(word), 2);

    switch (opcode(word))
    {
    case 0x1:
        return "LOAD " + R + " <- [" + XY + "]";
    case 0x2:
        return "LOAD " + R + " <- " + XY;
    case 0x3:
        if (operandXY(word) == 0xFF)
            return "STORE " + R + " -> SCREEN";
        return "STORE " + R + " -> [" + XY + "]";
    case 0x4:
        return "MOVE " + T + " <- " + S;
    case 0x5:
        return "ADD " + R + " <- " + S + " + " + T;
    case 0x6:
        return "FADD " + R + " <- " + S + " + " + T;
    case 0x7:
        return "OR " + R + " <- " + S + " | " + T;
    case 0x8:
        return "AND " + R + " <- " + S + " & " + T;
    case 0x9:
        return "XOR " + R + " <- " + S + " ^ " + T;
    case 0xA:
        return "ROTATE " + R + " right by " + std::to_string(nibbleT(word));
    case 0xB:
        if (nibbleR(word) == 0)
            return "JUMP to " + XY + " (always, R0 == R0)";
        return "JUMP to " + XY + " if " + R + " == R0";
    case 0xC:
        return "HALT";
    default:
        return "INVALID";
    }
}
