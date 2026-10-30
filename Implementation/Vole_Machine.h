#ifndef VOLE_MACHINE_H
#define VOLE_MACHINE_H

#include<cstdint>
#include<vector>
#include<string>
#include<memory> 
#include<map>
#include<functional>

class Cpu;
class Instruction;

using InstructionCreator =
        std::function<std::unique_ptr<Instruction>(uint16_t)>;
enum class StepStatus {
    OK, 
    HALTED,
    INVALID_INSTRUCTION,
    INVALID_PROGRAM_COUNTER,
    STEP_LIMIT_REACHED
};

struct LoadResult {
    bool success;
    int lineNumber;
    std::string message;
    int wordsLoaded;
};

class RegisterFile {
    public:
        static constexpr int count = 16;
        uint8_t get(uint8_t index);
        void set(uint8_t index, uint8_t value);
        void clear();
    
    private:
        uint8_t Registers[count] = {};

    
};

class Screen{
    private:
        std::vector<uint8_t> Output;

    public:
        void write(uint8_t value);
        const std::vector<uint8_t>& getOutput();
        void clear();
};

class Memory{
    
    public: 
        
        Memory(Screen& screen);
        static constexpr int Size = 256;
        static constexpr uint8_t screenAddress = 0xFF; 
        static bool isEvenAddress(int adresss) ;
        static bool fits (int start, int byteCount);
        void write(uint8_t address, uint8_t vlaue);
        uint8_t read(uint8_t address);
        bool loadBytes(int start, const std::vector<uint8_t>& bytes);
        void clear();
    
    private: 
        uint8_t cells[Size] = {};
        Screen& screen;

};

class Instruction {
    private:
        uint16_t word;

    protected:
        Instruction(uint16_t word): word(word){};
        uint8_t regR();
        uint8_t regS();
        uint8_t regT();
        uint8_t operandXY();
    public:
        uint16_t getWord();
        virtual void execute(Cpu& cpu) = 0;
        virtual ~Instruction() = 0;

};

class LoadFromMemoryInstruction: public Instruction {
    public:
        static std::unique_ptr<Instruction> create (uint16_t word);
        void execute(Cpu& cpu);
};

class LoadImmediateInstruction:public Instruction {
    public:
        static std::unique_ptr<Instruction> create (uint16_t word);
        void execute(Cpu& cpu);
};

class StoreInstruction: public Instruction {
    public:
        static std::unique_ptr<Instruction> create (uint16_t word);
        void execute(Cpu& cpu);
};

class MoveInstruction:public Instruction {
    public:
        static std::unique_ptr<Instruction> create (uint16_t word);
        void execute(Cpu& cpu);
};

class JumpInstruction:public Instruction {
    public:
        static std::unique_ptr<Instruction> create (uint16_t word);
        void execute(Cpu& cpu);
};

class RotationInstruction:public Instruction {
    public:
        static std::unique_ptr<Instruction> create (uint16_t word);
        void execute(Cpu& cpu);
};

class HaltInstruction:public Instruction {
    public:
        static std::unique_ptr<Instruction> create (uint16_t word);
        void execute(Cpu& cpu);
};

class BinaryRegisterInstruction:public Instruction {
    public:
        void execute(Cpu& cpu);
    protected:
        virtual uint8_t compute(uint8_t a, uint8_t b) = 0;
};

class AddInstruction: public BinaryRegisterInstruction {
    public:
        static std::unique_ptr<BinaryRegisterInstruction> create (uint16_t word);
        uint8_t compute(uint8_t a, uint8_t b);
};

class XorInstruction: public BinaryRegisterInstruction {
    public:
        static std::unique_ptr<BinaryRegisterInstruction> create (uint16_t word);
        uint8_t compute(uint8_t a, uint8_t b);
};

class OrInstruction: public BinaryRegisterInstruction {
    public:
        static std::unique_ptr<BinaryRegisterInstruction> create (uint16_t word);
        uint8_t compute(uint8_t a, uint8_t b);
};

class AddIntInstruction: public BinaryRegisterInstruction {
    public:
        static std::unique_ptr<BinaryRegisterInstruction> create (uint16_t word);
        uint8_t compute(uint8_t a, uint8_t b);
};

class AddFloatInstruction: public BinaryRegisterInstruction {
    public:
        static std::unique_ptr<BinaryRegisterInstruction> create (uint16_t word);
        uint8_t compute(uint8_t a, uint8_t b);
};

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

class InstructionDecoder {
    private:
        std::map<uint8_t, InstructionCreator> creators;
    public:
        InstructionDecoder();
        void registerInstruction(uint8_t opcode, InstructionCreator creator);
        std::unique_ptr<Instruction> decode(uint16_t word);
        bool isValid(uint16_t word);
};
class Cpu{
    
    private:
        RegisterFile registers;
        uint8_t programCounter;
        uint16_t instructionRegister;
        bool halted;
        Memory& memory;
        const InstructionDecoder& decoder;
        uint16_t fetch();
    
    public:
        Cpu(Memory& memory, const InstructionDecoder& decoder);
        StepStatus step();
        void reset();
        void halt();
        bool isHalted();
        uint8_t readRregister(uint8_t index);
        void writeRregister(uint8_t index, uint8_t value);
        uint8_t readMemory(uint8_t address);
        void writeMemory(uint8_t address, uint8_t value);
        const RegisterFile& getRegisters();
        uint8_t getProgrammCounter();
        void setProgrammCounter(uint8_t address);
        uint16_t getInstructionRegister();

};

#endif
