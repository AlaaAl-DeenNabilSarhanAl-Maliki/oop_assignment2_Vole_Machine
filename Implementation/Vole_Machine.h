#ifndef VOLE_MACHINE_H
#define VOLE_MACHINE_H

#include<cstdint>
#include<vector>
#include<string>
#include<memory> 
#include<map>
#include<functional>

// Forward declarations (needed because these types are used before their definitions)
class Instruction;
class Cpu;

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
        uint8_t get(uint8_t index) const;
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
        const std::vector<uint8_t>& getOutput() const;
        void clear();
};

class Memory{
    
    public: 
        
        Memory(Screen& screen);
        static constexpr int Size = 256;
        static constexpr uint8_t screenAddress = 0xFF; 
        static bool isEvenAddress(int address) ;
        static bool fits (int start, int byteCount);
        void write(uint8_t address, uint8_t value);
        uint8_t read(uint8_t address) const;
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
        Instruction(uint16_t word): word(word){}
        uint8_t regR() const;
        uint8_t regS() const;
        uint8_t regT() const;
        uint8_t operandXY() const;
    public:
        uint16_t getWord() const;
        virtual void execute(Cpu& cpu) = 0;
        virtual ~Instruction() = default;

};

class LoadFromMemoryInstruction: public Instruction {
    public:
        explicit LoadFromMemoryInstruction(uint16_t word): Instruction(word) {}
        static std::unique_ptr<Instruction> create (uint16_t word);
        void execute(Cpu& cpu) override;
};

class LoadImmediateInstruction:public Instruction {
    public:
        explicit LoadImmediateInstruction(uint16_t word): Instruction(word) {}
        static std::unique_ptr<Instruction> create (uint16_t word);
        void execute(Cpu& cpu) override;
};

class StoreInstruction: public Instruction {
    public:
        explicit StoreInstruction(uint16_t word): Instruction(word) {}
        static std::unique_ptr<Instruction> create (uint16_t word);
        void execute(Cpu& cpu) override;
};

class MoveInstruction:public Instruction {
    public:
        explicit MoveInstruction(uint16_t word): Instruction(word) {}
        static std::unique_ptr<Instruction> create (uint16_t word);
        void execute(Cpu& cpu) override;
};

class JumpInstruction:public Instruction {
    public:
        explicit JumpInstruction(uint16_t word): Instruction(word) {}
        static std::unique_ptr<Instruction> create (uint16_t word);
        void execute(Cpu& cpu) override;
};

class RotationInstruction:public Instruction {
    public:
        explicit RotationInstruction(uint16_t word): Instruction(word) {}
        static std::unique_ptr<Instruction> create (uint16_t word);
        void execute(Cpu& cpu) override;
};

class HaltInstruction:public Instruction {
    public:
        explicit HaltInstruction(uint16_t word): Instruction(word) {}
        static std::unique_ptr<Instruction> create (uint16_t word);
        void execute(Cpu& cpu) override;
};

class BinaryRegisterInstruction:public Instruction {
    public:
        void execute(Cpu& cpu) override;
    protected:
        explicit BinaryRegisterInstruction(uint16_t word): Instruction(word) {}
        virtual uint8_t compute(uint8_t a, uint8_t b) = 0;
};

class AndInstruction: public BinaryRegisterInstruction {
    public:
        explicit AndInstruction(uint16_t word): BinaryRegisterInstruction(word) {}
        static std::unique_ptr<BinaryRegisterInstruction> create (uint16_t word);
        uint8_t compute(uint8_t a, uint8_t b) override;
};

class XorInstruction: public BinaryRegisterInstruction {
    public:
        explicit XorInstruction(uint16_t word): BinaryRegisterInstruction(word) {}
        static std::unique_ptr<BinaryRegisterInstruction> create (uint16_t word);
        uint8_t compute(uint8_t a, uint8_t b) override;
};

class OrInstruction: public BinaryRegisterInstruction {
    public:
        explicit OrInstruction(uint16_t word): BinaryRegisterInstruction(word) {}
        static std::unique_ptr<BinaryRegisterInstruction> create (uint16_t word);
        uint8_t compute(uint8_t a, uint8_t b) override;
};

class AddIntInstruction: public BinaryRegisterInstruction {
    public:
        explicit AddIntInstruction(uint16_t word): BinaryRegisterInstruction(word) {}
        static std::unique_ptr<BinaryRegisterInstruction> create (uint16_t word);
        uint8_t compute(uint8_t a, uint8_t b) override;
};

class AddFloatInstruction: public BinaryRegisterInstruction {
    public:
        explicit AddFloatInstruction(uint16_t word): BinaryRegisterInstruction(word) {}
        static std::unique_ptr<BinaryRegisterInstruction> create (uint16_t word);
        uint8_t compute(uint8_t a, uint8_t b) override;
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
        std::unique_ptr<Instruction> decode(uint16_t word) const;
        bool isValid(uint16_t word) const;
};
class Cpu{
    
    private:
        RegisterFile registers;
        uint8_t programCounter;
        uint16_t instructionRegister;
        bool halted;
        Memory& memory;
        InstructionDecoder& decoder;
        uint16_t fetch();
    
    public:
        Cpu(Memory& memory, InstructionDecoder& decoder);
        StepStatus step();
        void reset();
        void halt();
        bool isHalted() const;
        uint8_t readRegister(uint8_t index) const;
        void writeRegister(uint8_t index, uint8_t value);
        uint8_t readMemory(uint8_t address) const;
        void writeMemory(uint8_t address, uint8_t value);
        const RegisterFile& getRegisters() const;
        uint8_t getProgramCounter() const;
        void setProgramCounter(uint8_t address);
        uint16_t getInstructionRegister() const;

};

class ProgramLoader {
    private:
        const InstructionDecoder& decoder;
        bool readWords(std::string path, std::vector<uint16_t>& words,LoadResult& result );
        bool ParseHexWord(std::string text, uint16_t& word);
        bool validateAll(const std::vector<uint16_t>& words, int startAddress, LoadResult& result);
    
    public:
        ProgramLoader(const InstructionDecoder& decoder);
        LoadResult load(std::string, int startAddress, Memory& memory);

};

class Machine {
    private:
        Screen screen;
        Memory memory;
        InstructionDecoder decoder;
        Cpu cpu;
        ProgramLoader loader;
    
    public:
        Machine();
        LoadResult loadProgram(std::string, int startAddress);
        bool setProgramCounter(uint8_t address);
        bool writeRegister(uint8_t index, uint8_t value);
        void writeMemory(uint8_t address, uint8_t value);
        StepStatus step();
        StepStatus runAll(int maxSteps = 100000);
        void reset();
        const Memory& getMemory() const;
        const Screen& getScreen() const;
        const Cpu& getCpu() const;

};
class ConsoleView {
public:
    void showState(const Machine& machine);
    void showMemory(const Memory& memory);
    void showRegisters(const Cpu& cpu);
    void showScreen(const Screen& screen);
    void showStatus(StepStatus status);
    void showLoadResult(const LoadResult& result);

private:
    std::string toHex(int value, int digits);
};

class ConsoleUI {
private:
    Machine& machine;
    ConsoleView view;

public:
    ConsoleUI(Machine& machine);
    void run();

private:
    void showMenu();
    void handleLoadProgram();
    void handleSetProgramCounter();
    void handleEditRegister();
    void handleEditMemory();
    void handleDisplayState();
    void handleExecute();
    void runAtOnce();
    void runStepByStep();
    void waitForEnter();
    bool askExitOrLoadAnother();
    int readHexValue(std::string prompt, int maxValue);
};

#endif
