cmake_minimum_required(VERSION 3.10)
project(VoleMachine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_CLI "Build the CLI application" ON)
option(BUILD_GUI "Build the GUI application" OFF)

add_subdirectory(Core)

if(BUILD_CLI)
    add_subdirectory(CLI)
endif()

if(BUILD_GUI)
    add_subdirectory(GUI)
endif()