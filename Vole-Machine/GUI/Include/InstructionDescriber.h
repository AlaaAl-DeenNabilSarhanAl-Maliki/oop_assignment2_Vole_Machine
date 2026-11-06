// InstructionDescriber.h
#ifndef VOLE_GUI_INSTRUCTIONDESCRIBER_H
#define VOLE_GUI_INSTRUCTIONDESCRIBER_H

#include <cstdint>
#include <string>

// GUI-side disassembler. Pure C++ (no Qt), so it can be unit-tested.
// It only reads the 16-bit word; it never touches the Machine.
class InstructionDescriber
{
public:
    // e.g. 0x2005 -> "LOAD R0 <- 05"
    //      0x3AFF -> "STORE RA -> SCREEN"
    //      0xB43C -> "JUMP to 3C if R4 == R0"
    //      0xC000 -> "HALT"
    // Returns "INVALID" / "DATA" style text when the pattern is not a Vole instruction.
    static std::string describe(uint16_t word);

    // Short mnemonic only: "LOAD", "STORE", "MOVE", "ADD", "FADD",
    // "OR", "AND", "XOR", "ROTATE", "JUMP", "HALT", or "???"
    static std::string mnemonic(uint16_t word);

    // True when the word matches a supported instruction pattern
    // (including the "unused digits must be zero" rules).
    static bool isValidPattern(uint16_t word);

private:
    InstructionDescriber() = delete;

    static uint8_t opcode(uint16_t word); // highest nibble
    static uint8_t nibbleR(uint16_t word);
    static uint8_t nibbleS(uint16_t word);
    static uint8_t nibbleT(uint16_t word);
    static uint8_t operandXY(uint16_t word);
    static std::string hex(unsigned value, int digits);
    static std::string reg(uint8_t index); // "R0".."RF"
};

#endif