#include <iostream>
#include <chrono>
#include <sys/stat.h>
#include "hardware/ChipEight.h"

/**
 * Checks if a file exists at the given path
 * @param path Path to file
 * @return True if exists, false otherwise
 */
bool file_exists(const char *path)
{
    struct stat buffer{};
    return (stat(path, &buffer) == 0);
}

int main(int argc, char **args)
{
    // Ensure correct number of args are supplied
    if (argc != 3)
    {
        std::cout << "ERROR: Requires 2 args: <rom_path> <cycle_delay>";
        exit(-1);
    }

    // Extract command line args
    const char *name = args[1];
    float delay = std::stof(args[2]);

    // Check ROM file exists
    if (!file_exists(name))
    {
        std::cout << "ROM DOES NOT EXIST: " << name << std::endl;
        exit(-1);
    }

    // Set up Chip-8, load the ROM and create the SDL window
    ChipEight chipEight;
    chipEight.LoadROM(name);
    chipEight.setupScreen("pong", 1);

    auto lastCycleTime = std::chrono::high_resolution_clock::now();

    // Emulation cycle
    while (chipEight.shouldRun)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (dt >= delay)
        {
            lastCycleTime = currentTime;
            chipEight.processInputs();
            chipEight.executeCycle();
            chipEight.updateScreen(chipEight.video, sizeof(chipEight.video[0]) * VIDEO_WIDTH);
        }
    }
    return 0;
}