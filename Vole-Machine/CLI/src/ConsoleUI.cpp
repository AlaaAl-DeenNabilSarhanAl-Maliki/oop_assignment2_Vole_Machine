#include "ConsoleUI.h"
#include "Machine.h"
#include <iostream>
#include <string>
#include <cctype>
#include <cstdint>

ConsoleUI::ConsoleUI(Machine &machine):machine(machine){}

void ConsoleUI::run()
{   
    bool isRunning = true;

    while (isRunning) {
        showMenu();
        int menuChoice = readHexValue("Choice: ", 6);

        switch (menuChoice) {
            case 1:
                handleLoadProgram();
                break;
            case 2:
                handleSetProgramCounter();
                break;
            case 3:
                handleEditRegister();
                break;
            case 4:
                handleEditMemory();
                break;
            case 5:
                handleDisplayState();
                break;
            case 6:
                if (handleExecute()) {
                    isRunning = false;
                }
                break;
            case 0:
                isRunning = false;
                break;
        }
    }

    std::cout << "Program ended.\n";
}

int ConsoleUI::readHexValue(std::string prompt, int maxValue)
{
   while (true) {
        std::cout << prompt;

        std::string inputText;
        std::getline(std::cin, inputText);

        bool isInputValid = !inputText.empty() && inputText.size() <= 4;

        for (size_t characterIndex = 0; characterIndex < inputText.size(); ++characterIndex) {
            unsigned char currentCharacter = static_cast<unsigned char>(inputText[characterIndex]);
            if (!std::isxdigit(currentCharacter)) {
                isInputValid = false;
            }
        }

        if (isInputValid) {
            int parsedValue = std::stoi(inputText, nullptr, 16);
            if (parsedValue <= maxValue) {
                return parsedValue;
            }
        }

        std::cout << "Invalid input, please try again.\n";
    }
}

void ConsoleUI::showMenu()
{
     std::cout << "\n========== Vole Machine ==========\n";
    std::cout << "1. Load program\n";
    std::cout << "2. Set program counter\n";
    std::cout << "3. Edit register\n";
    std::cout << "4. Edit memory\n";
    std::cout << "5. Display machine state\n";
    std::cout << "6. Execute program\n";
    std::cout << "0. Exit\n";
    std::cout << "==================================\n";
}

void ConsoleUI::handleLoadProgram()
{
    std::cout << "File path: ";
    std::string programFilePath;
    std::getline(std::cin, programFilePath);

    int startAddress = readHexValue("Start address (00-FC, even): ", 0xFC);
    if (startAddress % 2 != 0) {
        std::cout << "Start address must be an even number.\n";
        return;
    }

    LoadResult loadResult = machine.loadProgram(programFilePath, startAddress);
    view.showLoadResult(loadResult);
}

void ConsoleUI::handleSetProgramCounter()
{
    int newProgramCounter = readHexValue("New PC (00-FE): ", 0xFE);

    if (!machine.setProgramCounter(static_cast<uint8_t>(newProgramCounter))) {
        std::cout << "The machine rejected this program counter value.\n";
        return;
    }

    std::cout << "Program counter updated.\n";
}

void ConsoleUI::handleEditRegister()
{
    int registerIndex = readHexValue("Register (0-F): ", 0xF);
    int newValue = readHexValue("Value (00-FF): ", 0xFF);

    if (!machine.writeRegister(static_cast<uint8_t>(registerIndex), static_cast<uint8_t>(newValue))) {
        std::cout << "The machine rejected this register edit.\n";
        return;
    }

    std::cout << "Register updated.\n";
}

void ConsoleUI::handleEditMemory()
{
    int memoryAddress = readHexValue("Address (00-FF): ", 0xFF);
    int newValue = readHexValue("Value (00-FF): ", 0xFF);

    machine.writeMemory(static_cast<uint8_t>(memoryAddress), static_cast<uint8_t>(newValue));

    std::cout << "Memory updated.\n";
}

void ConsoleUI::handleDisplayState()
{
    view.showState(machine);
}

bool ConsoleUI::handleExecute()
{
    std::cout << "\n1. Run all at once\n";
    std::cout << "2. Run step by step\n";
    std::cout << "0. Back to menu\n";

    int executionChoice = readHexValue("Choice: ", 2);

    if (executionChoice == 0) {
        return false;
    }

    if (executionChoice == 1) {
        runAtOnce();
    } else {
        runStepByStep();
    }

    return askExitOrLoadAnother();
}

void ConsoleUI::runAtOnce()
{
    StepStatus finalStatus = machine.runAll();

    view.showStatus(finalStatus);
    view.showState(machine);
}

void ConsoleUI::runStepByStep()
{
    StepStatus currentStatus = StepStatus::OK;

    while (currentStatus == StepStatus::OK) {
        currentStatus = machine.step();

        view.showStatus(currentStatus);
        view.showState(machine);

        if (currentStatus == StepStatus::OK) {
            waitForEnter();
        }
    }
}

void ConsoleUI::waitForEnter()
{
    std::cout << "Press Enter to continue...";

    std::string ignoredInput;
    std::getline(std::cin, ignoredInput);
}

bool ConsoleUI::askExitOrLoadAnother()
{
    std::cout << "\n1. Load another program\n";
    std::cout << "0. Exit\n";

    int userChoice = readHexValue("Choice: ", 1);

    return userChoice == 0;
}
