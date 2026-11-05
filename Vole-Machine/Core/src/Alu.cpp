#include "Alu.h"

uint8_t Alu::addInt(uint8_t a, uint8_t b)
{
    return (a + b) % 256;
}

uint8_t Alu::addFloat(uint8_t a, uint8_t b)
{
    double the_sumA_B;
    double the_a = floatToDouble(a);
    double the_b = floatToDouble(b);
    the_sumA_B = the_a + the_b;
    return doubleToFloat(the_sumA_B);
}

uint8_t Alu::bitOR(uint8_t a, uint8_t b)
{
    return a | b;
}

uint8_t Alu::bitAnd(uint8_t a, uint8_t b)
{
    return a & b;
}

uint8_t Alu::bitXor(uint8_t a, uint8_t b)
{
    return a ^ b;
}

uint8_t Alu::rotateRight(uint8_t value, uint8_t times)
{
    times = times % 8;
    return static_cast<uint8_t>((value >> times) | (value << (8 - times)));
}
