#ifndef VOLE_SCREEN_H
#define VOLE_SCREEN_H

#include<cstdint>
#include<vector>

class Screen{
    private:
        std::vector<uint8_t> Output;

    public:
        void write(uint8_t value);
        const std::vector<uint8_t>& getOutput() const;
        void clear();
};

#endif