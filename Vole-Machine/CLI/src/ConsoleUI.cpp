#include "../Include/ConsoleUI.h"
#include "Machine.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdint>

namespace

{

    bool readLine(std::string &line)
    {
        return static_cast<bool>(std::getline(std::cin, line));
    }

    // Splits a command line into words. Text inside double quotes stays together
    // (and the quotes are removed), so paths with spaces work:  load "C:\My Files\a.txt" 00
    std::vector<std::string> tokenize(const std::string &line)
    {
        std::vector<std::string> tokens;
        std::string current;
        bool inQuotes = false;
        bool hasToken = false;

        for (char c : line)
        {
            if (c == '"')
            {
                inQuotes = !inQuotes;
                hasToken = true;
            }
            else if (std::isspace(static_cast<unsigned char>(c)) && !inQuotes)
            {
                if (hasToken)
                {
                    tokens.push_back(current);
                    current.clear();
                    hasToken = false;
                }
            }
            else
            {
                current += c;
                hasToken = true;
            }
        }
        if (hasToken)
        {
            tokens.push_back(current);
        }
        return tokens;
    }

    std::string toLower(std::string text)
    {
        for (char &c : text)
        {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        return text;
    }

    // Hex text ("3C" or "0x3C", 1-4 digits) -> value. False if invalid or above maxValue.
    bool parseHex(std::string text, int maxValue, int &value)
    {
        if (text.size() > 2 && text[0] == '0' && (text[1] == 'x' || text[1] == 'X'))
        {
            text = text.substr(2);
        }
        if (text.empty() || text.size() > 4)
        {
            return false;
        }

        int parsed = 0;
        for (unsigned char c : text)
        {
            if (!std::isxdigit(c))
            {
                return false;
            }
            parsed = parsed * 16 + (std::isdigit(c) ? c - '0' : std::toupper(c) - 'A' + 10);
        }
        if (parsed > maxValue)
        {
            return false;
        }
        value = parsed;
        return true;
    }

    // Plain decimal number (used for the step count).
    bool parseDecimal(const std::string &text, int &value)
    {
        if (text.empty() || text.size() > 6)
        {
            return false;
        }
        for (unsigned char c : text)
        {
            if (!std::isdigit(c))
            {
                return false;
            }
        }
        value = std::stoi(text);
        return true;
    }

    std::string hex(unsigned value, int digits)
    {
        std::ostringstream out;
        out << std::uppercase << std::hex << std::setw(digits) << std::setfill('0') << value;
        return out.str();
    }

    char printable(uint8_t value)
    {
        return (value >= 32 && value <= 126) ? static_cast<char>(value) : '.';
    }

    // The 16-bit instruction word stored at 'pc' (0 if it does not fit in memory).
    uint16_t wordAt(const Machine &machine, uint8_t pc)
    {
        if (pc == 0xFF)
        {
            return 0;
        }
        const Memory &memory = machine.getMemory();
        return static_cast<uint16_t>((memory.read(pc) << 8) |
                                     memory.read(static_cast<uint8_t>(pc + 1)));
    }

    // Everything the program has written to the screen so far.
    void printScreenOutput(const Machine &machine)
    {
        const std::vector<uint8_t> &output = machine.getScreen().getOutput();

        if (output.empty())
        {
            std::cout << "Screen output: (empty)\n";
            return;
        }

        std::cout << "Screen output: ";
        for (uint8_t value : output)
        {
            std::cout << printable(value);
        }
        std::cout << "\n   (hex:";
        for (uint8_t value : output)
        {
            std::cout << ' ' << hex(value, 2);
        }
        std::cout << ")\n";
    }

    // Prints only the screen bytes written since the last call ('shown' remembers how many).
    void printNewScreenBytes(const Machine &machine, std::size_t &shown)
    {
        const std::vector<uint8_t> &output = machine.getScreen().getOutput();

        if (output.size() < shown)
        {
            shown = 0; // the screen was cleared
        }
        for (; shown < output.size(); ++shown)
        {
            std::cout << "      screen <- " << hex(output[shown], 2)
                      << "  '" << printable(output[shown]) << "'\n";
        }
    }

} // namespace

ConsoleUI::ConsoleUI(Machine &machine) : machine(machine) {}

void ConsoleUI::run()
{
    std::cout << "Vole Machine - type 'help' to list the commands.\n";

    std::string line;
    while (true)
    {
        std::cout << "vole> " << std::flush;
        if (!readLine(line))
        {
            break; // input closed (EOF / Ctrl+Z)
        }

        const std::vector<std::string> args = tokenize(line);
        if (args.empty())
        {
            continue;
        }
        if (!execute(args))
        {
            break;
        }
    }

    std::cout << "Program ended.\n";
}

bool ConsoleUI::execute(const std::vector<std::string> &args)
{
    const std::string command = toLower(args[0]);

    if (command == "quit" || command == "exit" || command == "q")
    {
        return false;
    }
    else if (command == "help" || command == "h" || command == "?")
    {
        printHelp();
    }
    else if (command == "load")
    {
        cmdLoad(args);
    }
    else if (command == "step" || command == "s")
    {
        cmdStep(args);
    }
    else if (command == "run" || command == "r")
    {
        cmdRun();
    }
    else if (command == "restart")
    {
        cmdRestart();
    }
    else if (command == "reset")
    {
        cmdReset();
    }
    else if (command == "pc")
    {
        cmdSetProgramCounter(args);
    }
    else if (command == "setreg")
    {
        cmdSetRegister(args);
    }
    else if (command == "setmem")
    {
        cmdSetMemory(args);
    }
    else if (command == "regs" || command == "reg")
    {
        view.showRegisters(machine.getCpu());
    }
    else if (command == "mem")
    {
        view.showMemory(machine.getMemory());
    }
    else if (command == "screen")
    {
        view.showScreen(machine.getScreen());
    }
    else if (command == "state")
    {
        view.showState(machine);
    }
    else
    {
        std::cout << "Unknown command '" << args[0] << "'. Type 'help'.\n";
    }
    return true;
}

void ConsoleUI::printHelp() const
{
    std::cout
        << "  load <file> [start]    load a program; start = even hex address 00-FC (default 00)\n"
        << "                         put the path in \"quotes\" if it contains spaces\n"
        << "  step [n]   (s)         execute n instructions (decimal, default 1)\n"
        << "  run        (r)         run until HALT\n"
        << "  restart                reload the same file from the beginning\n"
        << "  reset                  clear memory, registers and screen\n"
        << "  pc <addr>              set the program counter (hex, even)\n"
        << "  setreg <R> <value>     write a register, e.g.  setreg A 2F\n"
        << "  setmem <addr> <value>  write a memory cell (hex)\n"
        << "  regs                   show the registers and PC / IR\n"
        << "  mem                    show the 256 memory cells\n"
        << "  screen                 show what the program wrote to the screen\n"
        << "  state                  show registers + memory + screen\n"
        << "  help                   this text\n"
        << "  quit       (q)         leave\n";
}

// Clean load: reset, load, then point the PC at the first instruction.
bool ConsoleUI::loadFile(const std::string &path, int startAddress)
{
    machine.reset();
    screenShown = 0;

    LoadResult loadResult = machine.loadProgram(path, startAddress);
    view.showLoadResult(loadResult);

    if (!loadResult.success)
    {
        machine.reset(); // a failed load can leave partial data behind
        currentFile.clear();
        return false;
    }

    machine.setProgramCounter(static_cast<uint8_t>(startAddress));
    currentFile = path;
    loadedStart = startAddress;
    return true;
}

void ConsoleUI::cmdLoad(const std::vector<std::string> &args)
{
    if (args.size() < 2)
    {
        std::cout << "Usage: load <file> [start]\n";
        return;
    }
    if (args.size() > 3)
    {
        std::cout << "Too many arguments. Put the file path in \"double quotes\" "
                     "if it contains spaces.\n";
        return;
    }

    int startAddress = 0;
    if (args.size() == 3 && !parseHex(args[2], 0xFC, startAddress))
    {
        std::cout << "Invalid start address '" << args[2] << "' (hex 00-FC, even).\n";
        return;
    }
    if (startAddress % 2 != 0)
    {
        std::cout << "Start address must be an even number.\n";
        return;
    }

    loadFile(args[1], startAddress);
}

void ConsoleUI::cmdStep(const std::vector<std::string> &args)
{
    int stepsToRun = 1;
    if (args.size() > 1 && (!parseDecimal(args[1], stepsToRun) || stepsToRun < 1))
    {
        std::cout << "Usage: step [n]   (n = number of steps, 1 or more)\n";
        return;
    }

    StepStatus status = StepStatus::OK;
    for (int i = 0; i < stepsToRun && status == StepStatus::OK; ++i)
    {
        const uint8_t pc = machine.getCpu().getProgramCounter();
        const uint16_t word = wordAt(machine, pc);

        status = machine.step();

        std::cout << "  " << hex(pc, 2) << ": " << hex(word, 4) << "\n";
        printNewScreenBytes(machine, screenShown);
    }

    std::cout << "PC = " << hex(machine.getCpu().getProgramCounter(), 2) << "\n";

    if (status != StepStatus::OK)
    {
        view.showStatus(status); // HALTED, an error, or the step limit
    }
}

void ConsoleUI::cmdRun()
{
    StepStatus finalStatus = machine.runAll();
    screenShown = machine.getScreen().getOutput().size();

    view.showStatus(finalStatus);
    printScreenOutput(machine);
    std::cout << "PC = " << hex(machine.getCpu().getProgramCounter(), 2)
              << "   (regs / mem / state show the machine)\n";
}

void ConsoleUI::cmdRestart()
{
    if (currentFile.empty())
    {
        std::cout << "Nothing to restart: load a program first.\n";
        return;
    }
    loadFile(currentFile, loadedStart);
}

void ConsoleUI::cmdReset()
{
    machine.reset();
    currentFile.clear();
    loadedStart = 0;
    screenShown = 0;
    std::cout << "Machine reset.\n";
}

void ConsoleUI::cmdSetProgramCounter(const std::vector<std::string> &args)
{
    int address = 0;
    if (args.size() != 2 || !parseHex(args[1], 0xFF, address))
    {
        std::cout << "Usage: pc <addr>   (hex 00-FF)\n";
        return;
    }

    if (!machine.setProgramCounter(static_cast<uint8_t>(address)))
    {
        std::cout << "Cannot set PC to " << hex(address, 2)
                  << ": it must be an even address below FF.\n";
        return;
    }
    std::cout << "PC = " << hex(address, 2) << "\n";
}

void ConsoleUI::cmdSetRegister(const std::vector<std::string> &args)
{
    int registerIndex = 0;
    int newValue = 0;
    if (args.size() != 3 || !parseHex(args[1], 0xF, registerIndex) ||
        !parseHex(args[2], 0xFF, newValue))
    {
        std::cout << "Usage: setreg <R> <value>   (R = 0-F, value = 00-FF)\n";
        return;
    }

    if (!machine.writeRegister(static_cast<uint8_t>(registerIndex), static_cast<uint8_t>(newValue)))
    {
        std::cout << "The machine rejected this register edit.\n";
        return;
    }
    std::cout << "R" << hex(registerIndex, 1) << " = " << hex(newValue, 2) << "\n";
}

void ConsoleUI::cmdSetMemory(const std::vector<std::string> &args)
{
    int address = 0;
    int newValue = 0;
    if (args.size() != 3 || !parseHex(args[1], 0xFF, address) ||
        !parseHex(args[2], 0xFF, newValue))
    {
        std::cout << "Usage: setmem <addr> <value>   (both hex 00-FF)\n";
        return;
    }

    machine.writeMemory(static_cast<uint8_t>(address), static_cast<uint8_t>(newValue));
    std::cout << "[" << hex(address, 2) << "] = " << hex(newValue, 2);
    if (address == 0xFF)
    {
        std::cout << "   (cell FF is the screen: the value was printed on it)";
    }
    std::cout << "\n";
}