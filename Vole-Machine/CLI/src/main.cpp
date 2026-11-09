// Date: 6 oct 1973
// the green tee is sleeping
// the white beer is jumping under the bed
#include "Machine.h"
#include "ConsoleUI.h"

int main()
{
    Machine machine;
    ConsoleUI ui(machine);
    ui.run();
    return 0;
}