#include "ProgramLoader.h"
<<<<<<< HEAD
#include "Common.h"
#include "Memory.h"
=======
#include "Machine.h"
>>>>>>> 9745b791b470c8b89481486ba49da46baf7d4a08
#include "InstructionDecoder.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <string>
#include <vector>

std::string ProgramLoader::toHexText(int value, int digitCount)
{
    std::stringstream hexStream;
    hexStream << std::hex << std::uppercase
              << std::setfill('0') << std::setw(digitCount) << value;
    return hexStream.str();
}

ProgramLoader::ProgramLoader(const InstructionDecoder &decoder) : decoder(decoder)
{
}

LoadResult ProgramLoader::load(std::string path, int startAddress, Memory &memory)
{
    LoadResult result;
    result.success = false;
    result.lineNumber = 0;
    result.wordsLoaded = 0;

    std::vector<uint16_t> words;
    std::vector<int> lineNumbers;

    if (!readWords(path, words, lineNumbers, result))
    {
        return result;
    }

    if (!validateAll(words, lineNumbers, startAddress, result))
    {
        return result;
    }

    int currentAddress = startAddress;
    for (size_t instructionIndex = 0; instructionIndex < words.size(); ++instructionIndex)
    {
        uint8_t highByte = static_cast<uint8_t>((words[instructionIndex] >> 8) & 0xFF);
        uint8_t lowByte = static_cast<uint8_t>(words[instructionIndex] & 0xFF);

        memory.write(currentAddress, highByte);
        memory.write(currentAddress + 1, lowByte);

        currentAddress += 2;
    }

    result.success = true;
    result.wordsLoaded = static_cast<int>(words.size());
    result.message = "Successfully loaded " + std::to_string(words.size()) +
                     " instructions starting at address 0x" + toHexText(startAddress, 2);

    return result;
}

bool ProgramLoader::readWords(std::string path, std::vector<uint16_t> &words, std::vector<int> &lineNumbers, LoadResult &result)
{
    std::ifstream programFile(path);
    if (!programFile.is_open())
    {
        result.success = false;
        result.message = "Could not open file: " + path;
        result.lineNumber = 0;
        return false;
    }

    const std::string byteOrderMark = "\xEF\xBB\xBF";
    std::string lineText;
    int currentLineNumber = 0;

    while (std::getline(programFile, lineText))
    {
        currentLineNumber++;

        if (currentLineNumber == 1 && lineText.compare(0, 3, byteOrderMark) == 0)
        {
            lineText.erase(0, 3);
        }

        size_t commentStartIndex = lineText.find(';');
        if (commentStartIndex != std::string::npos)
        {
            lineText = lineText.substr(0, commentStartIndex);
        }

        std::stringstream lineStream(lineText);
        std::string firstWordInLine;
        if (!(lineStream >> firstWordInLine))
        {
            continue;
        }

        uint16_t instructionWord;
        if (ParseHexWord(firstWordInLine, instructionWord))
        {
            words.push_back(instructionWord);
            lineNumbers.push_back(currentLineNumber);
        }
        else
        {
            result.success = false;
            result.lineNumber = currentLineNumber;
            result.message = "Invalid hex token: '" + firstWordInLine + "' at line " + std::to_string(currentLineNumber);
            programFile.close();
            return false;
        }
    }

    programFile.close();

    if (words.empty())
    {
        result.success = false;
        result.lineNumber = 0;
        result.message = "File is empty or contains no valid instructions.";
        return false;
    }

    return true;
}

bool ProgramLoader::ParseHexWord(std::string text, uint16_t &word)
{
    if (text.size() != 4)
    {
        return false;
    }

    for (size_t characterIndex = 0; characterIndex < text.size(); ++characterIndex)
    {
        unsigned char currentCharacter = static_cast<unsigned char>(text[characterIndex]);
        if (!std::isxdigit(currentCharacter))
        {
            return false;
        }
    }

    word = static_cast<uint16_t>(std::stoi(text, nullptr, 16));
    return true;
}

bool ProgramLoader::validateAll(const std::vector<uint16_t> &words, const std::vector<int> &lineNumbers, int startAddress, LoadResult &result)
{
    if (startAddress < 0 || startAddress % 2 != 0)
    {
        result.success = false;
        result.lineNumber = 0;
        result.message = "Start address must be an even number.";
        return false;
    }

    int instructionCount = static_cast<int>(words.size());
    int endAddress = startAddress + instructionCount * 2;

    if (endAddress > 255)
    {
        result.success = false;
        result.lineNumber = 0;
        result.message = "Memory Overflow: Program exceeds memory bounds (Start: 0x" +
                         toHexText(startAddress, 2) + ")";
        return false;
    }

    for (size_t instructionIndex = 0; instructionIndex < words.size(); ++instructionIndex)
    {
        if (!decoder.isValid(words[instructionIndex]))
        {
            result.success = false;
            result.lineNumber = lineNumbers[instructionIndex];
            result.message = "Invalid opcode/instruction pattern: '0x" +
                             toHexText(words[instructionIndex], 4) + "'";
            return false;
        }
    }

    return true;
}
