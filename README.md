# Chip-8 Emulator
A basic Chip-8 emulator (interpreter) written in C++ for Windows using SDL2 for rendering and audio.

![](preview.gif)


# Features
* 34 instructions as per this [Chip-8 specification](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#3.1) (first instruction is not necessary as stated)
* Toggleable "quirks" for certain shift, load and store instructions (on by default) which change their behaviour - some programs worked using incorrect assumptions about how certain instructions worked
* Working sound

**Note**: To pass some tests, such as the ["BC test"](https://slack-files.com/T3CH37TNX-F3RKEUKL4-b05ab4930d), the quirks
must be turned **off**


# Usage
## Building
1. Install mingw-w64 using [MSYS2](https://www.msys2.org/) (or otherwise)
2. Install SDL2 package ([libsdl2-dev](https://packages.msys2.org/package/mingw-w64-x86_64-SDL2))
3. Build via CMake
## Running
Run via `chip_8emu.exe <path_to_rom> <cycles_per_step>`

A decent default value for `cycles_per_step` is **8** on most games - should ideally be tweaked
manually for each game.

If it complains about the SDL2.dll being missing you must place it beside
the executable. You can find it at `<path_to_MSYS2_install>/msys64/mingw64/bin` or on
the [SDL2 website](https://www.libsdl.org/download-2.0.php).