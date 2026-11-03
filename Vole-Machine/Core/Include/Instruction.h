#ifndef VOLE_INSTRUCTION_H
#define VOLE_INSTRUCTION_H

#include<cstdint>

// Forward declarations (needed because these types are used before their definitions)
class Cpu;

class Instruction {
    private:
        uint16_t word;

    protected:
        Instruction(uint16_t word);
        uint8_t regR() const;
        uint8_t regS() const;
        uint8_t regT() const;
        uint8_t operandXY() const;
    public:
        uint16_t getWord() const;
        virtual void execute(Cpu& cpu) = 0;
        virtual ~Instruction() = default;

};

#endif