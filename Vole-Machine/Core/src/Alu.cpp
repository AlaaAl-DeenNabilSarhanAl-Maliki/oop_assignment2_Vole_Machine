#include "Alu.h"

uint8_t Alu::addInt(uint8_t a, uint8_t b)
{
    return (a + b) % 256;
}

uint8_t Alu::bitOR(uint8_t a, uint8_t b)
{
    return a | b;
}
