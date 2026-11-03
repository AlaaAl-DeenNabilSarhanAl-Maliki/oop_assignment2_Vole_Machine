#ifndef VOLE_ALU_H
#define VOLE_ALU_H

#include<cstdint>

class Alu{
    public:
        static uint8_t addInt(uint8_t a, uint8_t b);
        static uint8_t addFloat(uint8_t a, uint8_t b);
        static uint8_t bitOR(uint8_t a, uint8_t b);
        static uint8_t bitAnd(uint8_t a, uint8_t b);
        static uint8_t bitXor(uint8_t a, uint8_t b);
        static uint8_t rotateRight(uint8_t value, uint8_t times);
        static double floatToDouble(uint8_t bits);
        static uint8_t doubleToFloat(double value);
};

#endif