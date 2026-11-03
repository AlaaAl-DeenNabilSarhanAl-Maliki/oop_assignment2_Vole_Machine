#ifndef VOLE_COMMON_H
#define VOLE_COMMON_H

#include<cstdint>
#include<string>
#include<memory> 
#include<functional>
#include "Instruction.h"

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

#endif