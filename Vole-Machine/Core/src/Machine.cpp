#include "Vole_Machine.h"


Machine::Machine():screen(), 
        memory(screen), decoder(), 
        cpu(memory, decoder), 
        loader(decoder){}


//   use programLoader 
LoadResult Machine::loadProgram(  
    const std::string& path,
     int startAddress){

    return loader.load(path, startAddress, memory);
} 
// set PC  by the user
bool Machine::setProgramCounter(uint8_t address) {
    if (address %2 != 0 ) {
        return false;
    } 
    cpu.setProgramCounter(address);
    return true;

}

bool Machine::writeRegister(uint8_t index, uint8_t value){
    if (index >= RegisterFile::count) {
        return false;
    }
    cpu.writeRegister(index, value);
    return true;
}

void Machine::writeMemory(uint8_t address, uint8_t value) {
    cpu.writeMemory(address, value);
}

StepStatus Machine::step() {
    return cpu.step();
}

StepStatus Machine::runAll( int maxSteps) {
    for (int i = 0; i < maxSteps; i++) {
        
        StepStatus status = step();

        if (status != StepStatus::OK) {
            return status;
        }
    }
    return StepStatus::STEP_LIMIT_REACHED;
}

void Machine::reset() {
    cpu.reset();
}

 // console view need these three objects 
 
const Memory& Machine::getMemory() const {
    return memory;
}

const Screen& Machine::getScreen() const{
    return screen;
}

const Cpu& Machine::getCpu() const{
    return cpu;
}

