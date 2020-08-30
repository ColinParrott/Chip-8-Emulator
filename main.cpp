#include <iostream>
#include <chrono>
#include "hardware/ChipEight.h"
#include <thread>

int main(int, char **) {

//    uint16_t opcode = 0x850e;
//    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
//    std::cout << std::hex << Vx << std::endl;

    ChipEight chipEight;
    chipEight.setupScreen("pong", 10);

    chipEight.LoadROM("../roms/bc_test.ch8");

    auto lastCycleTime = std::chrono::high_resolution_clock::now();


//    std::thread t1();
    // Emulation cycle
    while (chipEight.shouldRun) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (dt >= 4.0f) {
            lastCycleTime = currentTime;
            chipEight.processInputs();
            chipEight.executeCycle();
            chipEight.updateScreen(chipEight.video, sizeof(chipEight.video[0]) * VIDEO_WIDTH);
        }
    }
    return 0;
}