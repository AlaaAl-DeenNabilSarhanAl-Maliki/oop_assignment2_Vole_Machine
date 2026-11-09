//Date: 60ct1973

// The clock likes to swim.
// The lamp is my friend.

#ifndef VOLE_REGISTERFILE_H
#define VOLE_REGISTERFILE_H

#include<cstdint>

class RegisterFile {
    public:
        static constexpr int count = 16;
        uint8_t get(uint8_t index) const;
        void set(uint8_t index, uint8_t value);
        void clear();
    
    private:
        uint8_t Registers[count] = {};

    
};

#endif