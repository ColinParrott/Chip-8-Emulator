cmake_minimum_required(VERSION 3.17)
project(chip8_emu)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -lmingw32")
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
set(SDL2_PATH "C:\\CLibraries\\SDL2-2.0.12\\i686-w64-mingw32")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)
find_package(SDL2 REQUIRED)


include_directories(${SDL2_INCLUDE_DIR})

add_executable(chip8_emu main.cpp hardware/ChipEight.cpp hardware/ChipEight.h)
target_link_libraries(chip8_emu ${SDL2_LIBRARY} -mwindows -mconsole)

