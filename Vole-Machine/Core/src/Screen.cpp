#include "../include/Screen.h"

void Screen::write(uint8_t value){
    Output.push_back(value);
}

const std::vector<uint8_t>& Screen::getOutput() const {
    return Output;
}

void Screen::clear() {
    Output.clear();
}