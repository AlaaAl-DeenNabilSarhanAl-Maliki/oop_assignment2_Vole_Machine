#include "Machine.h"
#include "ConsoleUI.h"

int main()
{
    Machine machine;
    ConsoleUI ui(machine);
    ui.run();
    return 0;
}