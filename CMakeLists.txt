cmake_minimum_required(VERSION 3.16)
project(chip8_emu)

#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})

add_subdirectory(tests)

add_executable(chip8_emu main.cpp hardware/ChipEight.cpp hardware/ChipEight.h hardware/Sound.cpp hardware/Sound.h)
target_link_libraries(chip8_emu ${SDL2_LIBRARY} -mwindows -mconsole)

