//Date: 60ct1973

// A bird sells blue chairs.
// The bed is running fast.

#ifndef VOLE_MEMORY_H
#define VOLE_MEMORY_H

#include<cstdint>
#include<vector>

class Screen;

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

#endif