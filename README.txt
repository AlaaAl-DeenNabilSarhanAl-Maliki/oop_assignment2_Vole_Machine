==========================================================
 VOLE MACHINE SIMULATOR
 CS213: OOP - Assignment 2 (2026-2027)
 Faculty of Computers and Artificial Intelligence,
 Cairo University
==========================================================

TEAM
----
1. Hatem Ahmed          - 20253050
2. Alaa Al-Deen Nabil   - 20253072
3. Mohammed Mazen       - 20253073
4. Mohammed Abdulmalik  - 20253081


DESCRIPTION
-----------
A simulator of the Vole machine written in C++ using an
object-oriented design. It has two front ends built on one
shared core:

  - CLI: a console (command-line) version
  - GUI: a graphical version (Qt)

The simulator loads a program from a text file into memory,
and runs it all at once or step by step. It displays the
256 memory cells, the 16 registers, the Program Counter (PC),
the Instruction Register (IR), and the screen output.
Programs containing invalid instructions are rejected.


REQUIREMENTS
------------
- A C++17 compiler (GCC, Clang, or MSVC)
- CMake 3.16 or later
- For the GUI only: Qt 6 (Widgets module)
  (the console version does not need Qt)

Tested on Windows with MSYS2 (UCRT64): g++, CMake and Qt 6.


HOW TO INSTALL AND BUILD
------------------------
1. Install a C++ compiler and CMake.
2. For the GUI, install Qt 6 (from qt.io or your package
   manager) and note its installation folder.
3. Open a terminal in the folder "Vole-Machine".
4. Run:

     cmake -S . -B build
     cmake --build build

   If CMake cannot find Qt, tell it where Qt is:

     cmake -S . -B build -DCMAKE_PREFIX_PATH="[path to Qt]"
     cmake --build build

On Windows with MSYS2 (the setup we used):
1. Install MSYS2 from msys2.org and open the
   "MSYS2 UCRT64" terminal.
2. Install the tools:

     pacman -S mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-qt6-base

3. Go to the folder "Vole-Machine" and build:

     cmake -S . -B build -G "MinGW Makefiles"
     cmake --build build

4. Run the programs from the same UCRT64 terminal (they need
   the Qt libraries in C:\msys64\ucrt64\bin).


HOW TO RUN
----------
Console version:   build/CLI/VoleCli
GUI version:       build/GUI/VoleGui

On Windows the files end with .exe.


HOW TO USE THE CONSOLE VERSION
------------------------------
Type "help" to list all commands. Main commands:

  load <file> [start]    load a program; start = even hex address 00-FC (default 00)
                         put the path in "quotes" if it contains spaces
  step [n]   (s)         execute n instructions (decimal, default 1)
  run        (r)         run until HALT
  restart                reload the same file from the beginning
  reset                  clear memory, registers and screen
  pc <addr>              set the program counter (hex, even)
  setreg <R> <value>     write a register, e.g.  setreg A 2F
  setmem <addr> <value>  write a memory cell (hex)
  regs                   show the registers and PC / IR
  mem                    show the 256 memory cells
  screen                 show what the program wrote to the screen
  state                  show registers + memory + screen
  help                   this text
  quit       (q)         leave


HOW TO USE THE GUI
------------------
1. Load a program file and choose its start address.
2. Press Step to execute one instruction, or RunAll to
   execute the whole program.
3. Memory, registers, PC, IR, and the screen update after
   each step.
4. To edit a register or a memory cell, double-click it,
   type the new value, and press Enter.
5. To change the theme, open the "View" menu in the
   top-left menu bar and choose a theme.
6. To see the instruction set, open the "Help" menu in
   the top-left menu bar.


PROGRAM FILE FORMAT
-------------------
- One instruction per line, written as four hex digits.
- Anything after the four digits on a line is ignored.
- Lines starting with ; and empty lines are ignored.

  2005 ; Load 05 into register 0
  2107 ; Load 07 into register 1
  5201 ; Add registers 0 and 1, result in register 2
  32FF ; Store register 2 to the screen (address FF)
  C000 ; Halt

Programs with invalid instructions are rejected, and the
line number of the error is reported.


SUPPORTED INSTRUCTIONS
----------------------
  1RXY  LOAD register R from memory cell XY
  2RXY  LOAD register R with the value XY
  3RXY  STORE register R into memory cell XY
        (3RFF writes to the screen)
  40RS  MOVE register R to register S
  5RST  ADD S and T (two's complement) into R
  6RST  ADD S and T (floating point) into R
  7RST  R = S OR T
  8RST  R = S AND T
  9RST  R = S XOR T
  AR0X  ROTATE register R right X times
  BRXY  JUMP to XY if register R equals register 0
  C000  HALT


SAMPLE PROGRAMS
---------------
The folder "programs" contains 20 test programs
(Test_1 to Test_20), including programs that the
simulator must reject.


SCREENSHOTS
-----------
See the "screenshots" folder.


VIDEOS
------
AI design process video:
https://drive.google.com/file/d/1ZTzM-34Vm5GzmK9NhiQBjDic2K8H6YDw/view?usp=drivesdk

GUI demo video:
https://drive.google.com/file/d/1imRqRnoSrwbunXqgO0hOCF3LU2qPRao_/view?usp=drivesdk