#include <iostream>
#include <chrono>
#include <sys/stat.h>
#include <string>
#include "hardware/ChipEight.h"


/**
 * Checks if a file exists at the given path
 * @param path Path to file
 * @return True if exists, false otherwise
 */
bool fileExists(const char *path)
{
    struct stat buffer{};
    return (stat(path, &buffer) == 0);
}

/**
 * Attempts to extracts rom name from a path
 * @param path Path to file
 * @return Name of file (if possible)
 */
std::string extractROMName(const char *path)
{
    std::string s = path;
    std::string result;
    int index = s.find_last_of("/\\");

    if (s.find_last_of("/\\") != std::string::npos)
    {
        // Name too short, no point in returning '/'
        if (index > s.length() - 2)
        {
            result = path;
        }

        std::string ex = s.substr(index + 1, s.length() - (index));
        return ex;
    }
    else
    {
        return path;
    }
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
    const char *path = args[1];
    int cyclesPerTick = std::stoi(args[2]);

    // Check ROM file exists
    if (!fileExists(path))
    {
        std::cout << "ROM DOES NOT EXIST: " << path << std::endl;
        exit(-1);
    }
    std::string title = "Chip-8: " + extractROMName(path);

    // Set up Chip-8, load the ROM and create the SDL window
    ChipEight chipEight(false, false, cyclesPerTick);
    chipEight.LoadROM(path);
    chipEight.setupScreen(title.c_str(), 20);

    auto lastCycleTime = std::chrono::high_resolution_clock::now();

    // Emulation cycle
    while (chipEight.shouldRun)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (dt >= (float) 1000 / 60)
        {
            lastCycleTime = currentTime;
            chipEight.processInputs();
            chipEight.executeCycle();
            chipEight.updateScreen(chipEight.video, sizeof(chipEight.video[0]) * VIDEO_WIDTH);
        }
    }
    return 0;
}