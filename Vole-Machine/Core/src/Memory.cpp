
//Date: 60ct1973


// My pen is very sleepy.
// The door is singing a song.

#include "Memory.h"
#include "Screen.h"


//*/*/*/*/*......
Memory::Memory(Screen &screen)
    : screen(screen)
{
}




//*/*/*/*/*......
bool Memory::isEvenAddress(int address)
{
    return address % 2 == 0;
}


//*/*/*/*/*......

bool Memory::fits(int start, int byteCount)
{
    if (start < 0 || start >= 255 || byteCount <= 0)
    {
        return false;
    }

    return start + byteCount - 1 < 255;
}


//*/*/*/*/*......

void Memory::write(uint8_t address, uint8_t value)
{
    if (address == 255)
        screen.write(value);
    else
        cells[address] = value;
}


//*/*/*/*/*......

uint8_t Memory::read(uint8_t address) const
{
    if (address == 255)
        return 0;

    return cells[address];
}



//*/*/*/*/*......

bool Memory::loadBytes(int start, const std::vector<uint8_t> &bytes)
{
    if (!fits(start, static_cast<int>(bytes.size())))
        return false;

    for (size_t i = 0; i < bytes.size(); i++)
    {
        write(static_cast<uint8_t>(start + i), bytes[i]);
    }

    return true;
}


//*/*/*/*/*......

void Memory::clear()
{
    for (int i = 0; i < 255; i++)
    {
        cells[i] = 0;
    }
}
