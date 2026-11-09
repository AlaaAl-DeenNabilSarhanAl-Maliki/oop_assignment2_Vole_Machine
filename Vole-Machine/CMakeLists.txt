cmake_minimum_required(VERSION 3.16)
project(VoleMachine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)          # std::optional
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(Core)
add_subdirectory(GUI)
add_subdirectory(CLI)