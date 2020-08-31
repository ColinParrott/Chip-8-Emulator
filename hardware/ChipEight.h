#ifndef CHIP8_EMU_CHIPEIGHT_H
#define CHIP8_EMU_CHIPEIGHT_H

#include <cstdint>
#include <SDL.h>
#include <random>

/**
 * Starting point in memory where programs can begin writing
 */
const static unsigned int START_ADDRESS = 0x200;

/**
 * Address where the font set starts at
 */
const static unsigned int FONT_START_ADDRESS = 0x0;

/**
 * Size  of font set (16 chars of 5 bytes, 16*5= 80)
 */
const static unsigned int FONT_SET_SIZE = 80;

/**
 * Font set data containing the numbers 0-9, and letters A-F - should be placed in memory at FONT_START_ADDRESS
 */
const static uint8_t fontset[FONT_SET_SIZE] =
        {
                0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                0x20, 0x60, 0x20, 0x20, 0x70, // 1
                0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
                0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
                0xF0, 0x10, 0x20, 0x40, 0x40, // 7
                0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
                0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
                0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
                0xF0, 0x80, 0x80, 0x80, 0xF0, // C
                0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

/**
 * Width and height of Chip-8 display
 */
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;

class ChipEight
{
private:
    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;
    uint16_t opcode{};
    uint8_t registers[16]{};
    uint16_t indexRegister{};
    uint16_t pc{};
    uint8_t sp{};
    uint8_t delayRegister{};
    uint8_t soundRegister{};
    uint8_t keypad[16]{};
    uint16_t stack[16]{};
    uint8_t memory[4096]{};
    bool loadStoreQuirk;
    bool shiftQuirk;

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

    void LoadROM(char const *path);

    void executeCycle();

    void processInputs();

    void updateScreen(const void *buffer, int pitch);

    void setupScreen(const char *title, unsigned int scale);

    void writeToMemory(int index, uint8_t value);

    ChipEight(bool loadStoreQuirk, bool shiftQuirk);

    ~ChipEight();
};


#endif //CHIP8_EMU_CHIPEIGHT_H
