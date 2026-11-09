// Date: 6 oct 1973
// the black blanket is walking around
// the cat is screaming on the sofa

#include "Instruction.h"

Instruction::Instruction(uint16_t word) : word(word) {}
///*/*/*/*
uint8_t Instruction::regR() const
{
    // Right shift  then bit mask
    return (word >> 8) & 0xF;
}
///*/*/*/*
uint8_t Instruction::regS() const
{
    // Right shift then bit mask
    return (word >> 4) & 0xF;
}
///*/*/*/*
uint8_t Instruction::regT() const
{
    // bit mask
    return word & 0xF;
}
///*/*/*/*
uint8_t Instruction::operandXY() const
{
    return word & 0xFF; // bit mask to get the last 8 bit
}
///*/*/*/*
uint16_t Instruction::getWord() const
{
    return word;
}
