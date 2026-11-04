#include"Vole_Machine.h"
#include <fstream>
#include <sstream>
#include "ProgramLoader.h"


ProgramLoader::ProgramLoader(const InstructionDecoder &decoder): decoder(decoder) 
{}

LoadResult ProgramLoader::load(std::string path, int startAddress, Memory &memory)
{
   LoadResult result;
    result.success = false;
    result.lineNumber = 0;
    result.wordsLoaded = 0;

    std::vector<uint16_t> words;

    
    if (!readWords(path, words, result)) {
        return result; 
    }

     
    if (!validateAll(words, startAddress, result)) {
        return result; 
    }

    
    int currentAddress = startAddress;
    for (size_t i = 0; i < words.size(); ++i) {
        
        uint8_t highByte = static_cast<uint8_t>((words[i] >> 8) & 0xFF);
        uint8_t lowByte  = static_cast<uint8_t>(words[i] & 0xFF);

    
        memory.write(currentAddress, highByte);
        memory.write(currentAddress + 1, lowByte);

        currentAddress += 2;
    }

    
    result.success = true;
    result.wordsLoaded = static_cast<int>(words.size());
    result.message = "Successfully loaded " + std::to_string(words.size()) + 
                     " instructions starting at address 0x" + std::to_string(startAddress);

    return result;
}

bool ProgramLoader::readWords(std::string path, std::vector<uint16_t> &words, LoadResult &result)
{
   std::ifstream file(path);
    if (!file.is_open()) {
        result.success = false;
        result.message = "Could not open file: " + path;
        result.lineNumber = 0;
        return false;
    }

    std::string line;
    int currentLine = 0;

    
    while (std::getline(file, line)) {
        currentLine++;
        std::stringstream ss(line);
        std::string token;

        
        while (ss >> token) {
            uint16_t word;
            
            
            if (ParseHexWord(token, word)) {
                words.push_back(word);
            } else {
                
                result.success = false;
                result.lineNumber = currentLine;
                result.message = "Invalid hex token: '" + token + "' at line " + std::to_string(currentLine);
                file.close();
                return false;
            }
        }
    }

    file.close();

    
    if (words.empty()) {
        result.success = false;
        result.lineNumber = 0;
        result.message = "File is empty or contains no valid instructions.";
        return false;
    }

    return true;
}

bool ProgramLoader::ParseHexWord(std::string text, uint16_t &word)
{
   if (text.empty()) return false;
    
    std::stringstream ss;
    ss << std::hex << text;
    
    unsigned int temp;
    if (ss >> temp) {
        word = static_cast<uint16_t>(temp);
        return true;
    }
    return false;
}

bool ProgramLoader::validateAll(const std::vector<uint16_t> &words, int startAddress, LoadResult &result)
{
   int requiredMemorySize = startAddress + (words.size() * 2);

    
    if (startAddress < 0 || requiredMemorySize > 256) {
        result.success = false;
        result.lineNumber = 0;
        result.message = "Memory Overflow: Program exceeds memory bounds (Start: " + 
                         std::to_string(startAddress) + ")";
        return false;
    }

    for (size_t i = 0; i < words.size(); ++i) {
        
        if (!decoder.decode(words[i])) {
            result.success = false;
            
            result.lineNumber = static_cast<int>(i + 1); 
            result.message = "Invalid opcode/instruction pattern: '0x" + 
                             std::to_string(words[i]) + "'";
            return false;
        }
    }

    return true;
}

