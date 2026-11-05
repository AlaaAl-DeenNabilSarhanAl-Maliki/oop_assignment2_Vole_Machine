#ifndef VOLE_PROGRAMLOADER_H
#define VOLE_PROGRAMLOADER_H

#include<cstdint>
#include<vector>
#include<string>
#include "Common.h"

class InstructionDecoder;
class Memory;

class ProgramLoader {
    private:
        const InstructionDecoder& decoder;
        bool readWords(std::string path, std::vector<uint16_t>& words, std::vector<int>& lineNumbers, LoadResult& result );
        bool ParseHexWord(std::string text, uint16_t& word);
        std::string toHexText(int value, int digitCount);
        bool validateAll(const std::vector<uint16_t>& words, const std::vector<int>& lineNumbers, int startAddress, LoadResult& result);
    
    public:
        ProgramLoader(const InstructionDecoder& decoder);
        LoadResult load(std::string, int startAddress, Memory& memory);

};

#endif