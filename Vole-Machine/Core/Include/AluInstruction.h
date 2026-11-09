// Date: 6 oct 1973
// the black donkey is walking around
// the last page is screaming on the bed
#ifndef VOLE_ALUINSTRUCTIONS_H
#define VOLE_ALUINSTRUCTIONS_H

#include <cstdint>
#include <memory>
#include "Instruction.h"

class RotationInstruction : public Instruction
{
public:
    explicit RotationInstruction(uint16_t word);
    static std::unique_ptr<Instruction> create(uint16_t word);
    void execute(Cpu &cpu) override;
};

class BinaryRegisterInstruction : public Instruction
{
public:
    void execute(Cpu &cpu) override;

protected:
    explicit BinaryRegisterInstruction(uint16_t word);
    virtual uint8_t compute(uint8_t a, uint8_t b) = 0;
};

class AndInstruction : public BinaryRegisterInstruction
{
public:
    explicit AndInstruction(uint16_t word);
    static std::unique_ptr<BinaryRegisterInstruction> create(uint16_t word);
    uint8_t compute(uint8_t a, uint8_t b) override;
};

class XorInstruction : public BinaryRegisterInstruction
{
public:
    explicit XorInstruction(uint16_t word);
    static std::unique_ptr<BinaryRegisterInstruction> create(uint16_t word);
    uint8_t compute(uint8_t a, uint8_t b) override;
};

class OrInstruction : public BinaryRegisterInstruction
{
public:
    explicit OrInstruction(uint16_t word);
    static std::unique_ptr<BinaryRegisterInstruction> create(uint16_t word);
    uint8_t compute(uint8_t a, uint8_t b) override;
};

class AddIntInstruction : public BinaryRegisterInstruction
{
public:
    AddIntInstruction(uint16_t);
    static std::unique_ptr<BinaryRegisterInstruction> create(uint16_t word);
    uint8_t compute(uint8_t a, uint8_t b) override;
};

class AddFloatInstruction : public BinaryRegisterInstruction
{
public:
    explicit AddFloatInstruction(uint16_t word);
    static std::unique_ptr<BinaryRegisterInstruction> create(uint16_t word);
    uint8_t compute(uint8_t a, uint8_t b) override;
};

#endif