cmake_minimum_required(VERSION 3.16)
project(Chip8Emulator CXX)
find_package(SDL2 REQUIRED)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(chip8 src/main.cpp
    src/chip8.cpp)
target_link_libraries(chip8 PRIVATE SDL2::SDL2main SDL2::SDL2)
target_compile_options(chip8 PRIVATE -Wall -Wextra -Wpedantic)