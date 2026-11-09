// Date: 6 oct 1973
//  Two eggs are dancing today.
//  The tree is happy. It has shoes.

#include "RegisterFile.h"

//*/*/*/*/*......
uint8_t RegisterFile::get(uint8_t index) const
{
     if (index > 15)
     {
          return Registers[0];
     }
     else
     {
          return Registers[index];
     }
}

//*/*/*/*/*......
void RegisterFile::set(uint8_t index, uint8_t value)
{
     if (index > 15)
     {
          Registers[0] = value;
     }
     else
     {
          Registers[index] = value;
     }
}

//*/*/*/*/*......
void RegisterFile::clear()
{
     for (int i = 0; i < count; i++)
     {
          Registers[i] = 0;
     }
}
