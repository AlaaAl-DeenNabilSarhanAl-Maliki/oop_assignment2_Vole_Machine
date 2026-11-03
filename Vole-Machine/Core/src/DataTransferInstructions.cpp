# include "Vole_Machine.h"

// implement LoadFromMemoryInstruction methods 
LoadFromMemoryInstruction::LoadFromMemoryInstruction(uint16_t word):Instruction(word){}

std::unique_ptr<Instruction> LoadFromMemoryInstruction::create(uint16_t word){
    return std::make_unique<LoadFromMemoryInstruction>(word); 
}

void LoadFromMemoryInstruction::execute(Cpu& cpu) {
    
    uint8_t address = operandXY();
    uint8_t value = cpu.readMemory(address);

    cpu.writeRegister(regR(), value);
}

// implement LoadImmediateInstruction methods 
LoadImmediateInstruction::LoadImmediateInstruction(uint16_t word)
    : Instruction(word) {}

std::unique_ptr<Instruction> LoadImmediateInstruction::create(uint16_t word) {
    return std::make_unique<LoadImmediateInstruction>(word);
}

void LoadImmediateInstruction::execute(Cpu& cpu) {
    uint8_t value = operandXY();

    cpu.writeRegister(regR(), value);
}

// implement StoreInstruction methods 

StoreInstruction::StoreInstruction(uint16_t word)
    : Instruction(word) {}

std::unique_ptr<Instruction> StoreInstruction::create(uint16_t word) {
    return std::make_unique<StoreInstruction>(word);
}

void StoreInstruction::execute(Cpu& cpu) {
    uint8_t address = operandXY();
    uint8_t value = cpu.readRegister(regR());

    cpu.writeMemory(address, value);
}

// implement MoveInstruction methods 
MoveInstruction::MoveInstruction(uint16_t word)
    : Instruction(word) {}

std::unique_ptr<Instruction> MoveInstruction::create(uint16_t word) {
    if ((word & 0x0F00) != 0)
        return nullptr;

    return std::make_unique<MoveInstruction>(word);
}

void MoveInstruction::execute(Cpu& cpu) {
    uint8_t value = cpu.readRegister(regR());

    cpu.writeRegister(regS(), value);
}
