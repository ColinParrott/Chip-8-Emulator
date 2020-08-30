#ifndef CHIP8_EMU_CHIPEIGHT_H
#define CHIP8_EMU_CHIPEIGHT_H

#include <cstdint>
#include <SDL.h>
#include <random>

const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;

class ChipEight {
private:
    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;
//    std::uniform_int_distribution<uint16_t> randInt;
//    std::uniform_int_distribution<int> randPixel;
    uint16_t opcode{};
    uint8_t registers[16]{};
    uint16_t indexRegister{};
    uint16_t pc{};
    uint8_t sp{};
    uint8_t delayRegister{};
    uint8_t soundRegister{};
    uint8_t keys[16]{};
    uint16_t stack[16]{};
    uint8_t memory[4096]{};

    void OP_00E0();

    void OP_00EE();

    void OP_1NNN();

    void OP_2NNN();

    void OP_3XKK();

    void OP_4XKK();

    void OP_5XY0();

    void OP_6XKK();

    void OP_7XKK();

    void OP_8XY0();

    void OP_8XY1();

    void OP_8XY2();

    void OP_8XY3();

    void OP_8XY4();

    void OP_8XY5();

    void OP_8XY6();

    void OP_8XY7();

    void OP_8XYE();

    void OP_9XY0();

    void OP_ANNN();

    void OP_BNNN();

    void OP_CXKK();

    void OP_DXYN();

    void OP_EX9E();

    void OP_EXA1();

    void OP_FX07();

    void OP_FX0A();

    void OP_FX15();

    void OP_FX18();

    void OP_FX1E();

    void OP_FX29();

    void OP_FX33();

    void OP_FX55();

    void OP_FX65();

    void OP_NULL();

    void executeOpCode();


public:

    bool shouldRun;
    bool drawFlag;

    // 32 bit int to work with SDL easier (could just be bool array
    // as Chip8 video is monochrome (black + white))
    uint32_t video[64 * 32]{};

    // SDL stuff
    SDL_Texture *texture{};
    SDL_Renderer *renderer{};
    SDL_Window *window{};

    void LoadROM(char const *filename);

    void executeCycle();

    void processInputs();

    void updateScreen(const void *buffer, int pitch) const;

    void setupScreen(const char *title, unsigned int scale);

    void writeToMemory(int index, uint8_t value);

    ChipEight();

    ~ChipEight();
};


#endif //CHIP8_EMU_CHIPEIGHT_H
