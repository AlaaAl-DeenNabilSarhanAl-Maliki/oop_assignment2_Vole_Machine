# Vole Machine Simulator

CS213: Programming II, Assignment 2 (2026-2027)
Faculty of Computers and Artificial Intelligence, Cairo University

A simulator for the **Vole machine**, a fictitious computer from *Computer Science: An Overview* (J. Glenn Brookshear), written in standard C++ using an object-oriented design. Two front ends share one core: a console interface (CLI) and a graphical interface (GUI).



## Features

- Loads a program from a text file into memory at a chosen **even** start address (`00`, `02`, ..., `FE`).
- Rejects programs that contain invalid instructions and reports the line number.
- 256 memory cells, 16 general-purpose registers, Program Counter (PC) and Instruction Register (IR).
- Runs a program all at once or step by step.
- Lets the user overwrite any register, memory cell, or the PC.
- Writing to address `FF` prints to the screen.
- Displays memory, registers, PC, IR and screen output.

## Supported Instructions

| Opcode | Operand | Description |
|--------|---------|-------------|
| 1 | RXY | LOAD register R from memory cell XY |
| 2 | RXY | LOAD register R with the value XY |
| 3 | RXY | STORE register R into memory cell XY (`3RFF` writes to the screen) |
| 4 | 0RS | MOVE register R to register S |
| 5 | RST | ADD S and T (two's complement), result in R |
| 6 | RST | ADD S and T (floating point), result in R |
| 7 | RST | R = S OR T |
| 8 | RST | R = S AND T |
| 9 | RST | R = S XOR T |
| A | R0X | ROTATE register R right X times |
| B | RXY | JUMP to XY if register R equals register 0 |
| C | 000 | HALT |

## Project Structure

```
oop_assignment2_Vole_Machine/
├── Final_design/            # PlantUML sources and diagrams
├── Initial Design Versions/ # Part 1 designs
├── Vole-Machine/
│   ├── Core/                # Simulator logic
│   ├── CLI/                 # Console interface
│   └── GUI/                 # Graphical interface
└── programs/                # Test_1 ... Test_20
```

## Requirements

- C++17 compiler (GCC, Clang or MSVC)
- CMake 3.16 or later
- GUI only: <Qt 6>

## Build

```bash
cd Vole-Machine
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/CLI/<cli-executable>
./build/GUI/<gui-executable>
```

## Program File Format

- One instruction per line, four hex digits.
- Text after the four digits is ignored.
- Lines starting with `;` and empty lines are ignored.

```
2005 ; Load 05 into register 0
2107 ; Load 07 into register 1
5201 ; Add registers 0 and 1, result in register 2
32FF ; Store register 2 to the screen
C000 ; Halt
```

## Sample Programs

`programs/` contains 20 test programs, including programs that must be rejected.

## Screenshots

<insert screenshots>

## Videos

- AI design process: <link>
- GUI demo: <link>

## Design

The final class diagram is in `Final_design/` and matches the implemented code.
