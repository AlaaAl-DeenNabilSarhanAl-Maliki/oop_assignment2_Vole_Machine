#include"Vole_Machine.h"


Cpu::Cpu(Memory& memory, InstructionDecoder& decoder):memory(memory),
decoder(decoder),
programCounter(0),
instructionRegister(0),
halted(false){}

 

uint8_t Cpu::getProgrammCounter(){
    return programCounter;
  }
void Cpu::setProgrammCounter(uint8_t address){
        programCounter= address;

  }

uint16_t Cpu::getInstructionRegister(){

    return instructionRegister;

  }

const RegisterFile& Cpu::getRegisters(){

      return registers;

  }
uint8_t Cpu::readRregister(uint8_t index){

     return   registers.get(index);

   }

void Cpu::writeRregister(uint8_t index, uint8_t value){
    
    registers.set(index,value);


   }

uint8_t Cpu::readMemory(uint8_t address){

    return memory.read(address);


    }

void Cpu::writeMemory(uint8_t address, uint8_t value){

    memory.write(address,value);


}

void Cpu::reset(){
    
    registers.clear();
    programCounter=0;
    instructionRegister=0;
    halted=false;
}

uint16_t combineBytes(uint8_t highByte, uint8_t lowByte) {
    return (static_cast<uint16_t>(highByte) << 8) | lowByte;
}



uint16_t Cpu::fetch() {
    uint8_t highByte = readMemory(programCounter);
    uint8_t lowByte = readMemory(programCounter + 1);

    instructionRegister= combineBytes(highByte, lowByte);

    programCounter += 2;
    return instructionRegister;
}

void Cpu::halt() {
    halted = true;
}

bool Cpu::isHalted() {
    return halted;
}

 StepStatus Cpu::step(){
    if(isHalted()){
        return StepStatus::HALTED ;
    }
    if (getProgrammCounter()>=254){

        return StepStatus::INVALID_PROGRAM_COUNTER;
    }

    std::unique_ptr<Instruction> decodedInstructon = decoder.decode(fetch());

    if (decodedInstructon == nullptr) {
        return StepStatus::INVALID_INSTRUCTION;
    }

     decodedInstructon->execute(*this);

    if (isHalted()) {
        return StepStatus::HALTED;
    }
    
    return StepStatus::OK;
 }
