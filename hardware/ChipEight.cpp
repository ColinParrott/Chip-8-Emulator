//
// Created by Colin on 28/08/2020.
//

#include "ChipEight.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <cstring>
#include <windows.h>

const static unsigned int START_ADDRESS = 0x200;
const static unsigned int FONT_START_ADDRESS = 0x0;
const static unsigned int FONTSET_SIZE = 80;
const static uint8_t fontset[FONTSET_SIZE] =
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

ChipEight::ChipEight() {
    // Set all vars to initial values
    opcode = -1;
    memset(registers, 0, sizeof(registers));
    indexRegister = 0;
    pc = START_ADDRESS;
    sp = 0;
    delayRegister = 0;
    soundRegister = 0;
    memset(keys, 0, sizeof(keys));
    memset(stack, 0, sizeof(stack));
    memset(memory, 0, sizeof(memory));

    // Load fontset into memory 0x00 - 0x50 (0 to 80)
    for (int i = 0; i < FONTSET_SIZE; i++) {
        memory[FONT_START_ADDRESS + i] = fontset[i];
    }

    shouldRun = true;
    drawFlag = false;
    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
}


void ChipEight::LoadROM(const char *filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file.is_open()) {
        // Get size of file and allocate a buffer to hold the contents
        std::streampos size = file.tellg();
        char *buffer = new char[size];

        // Go back to the beginning of the file and fill the buffer
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        // Load the ROM contents into the Chip8's memory, starting at 0x200
        for (long i = 0; i < size; ++i) {
            memory[START_ADDRESS + i] = buffer[i];
        }

        // Free the buffer
        delete[] buffer;
    }
}

void ChipEight::executeCycle() {
    // Opcode is 2 bytes long, so merge two successive bytes
    // Extend first byte to 16 bits (by shifting left 8 which pads 8 zeroes effectively), then
    // OR with next byte to replace padded zeroes with the second byte's value
    opcode = (memory[pc] << 8u) | memory[pc + 1];
//    std::cout << std::hex << opcode << std::endl;

    // Pre-emptively add 2 to PC, to move to next opcode
    pc += 2;

    executeOpCode();

    if (delayRegister > 0) {
        --delayRegister;
    }

    if (soundRegister > 0) {
        --soundRegister;
        if (soundRegister > 0) {
            Beep(420, 17);
        }
    }
}

void ChipEight::processInputs() {
    bool quit = false;

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: {
                quit = true;
            }
                break;

            case SDL_KEYDOWN: {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE: {
                        quit = true;
                    }
                        break;

                    case SDLK_x: {
                        keys[0] = 1;
                    }
                        break;

                    case SDLK_1: {
                        keys[1] = 1;
                    }
                        break;

                    case SDLK_2: {
                        keys[2] = 1;
                    }
                        break;

                    case SDLK_3: {
                        keys[3] = 1;
                    }
                        break;

                    case SDLK_q: {
                        keys[4] = 1;
                    }
                        break;

                    case SDLK_w: {
                        keys[5] = 1;
                    }
                        break;

                    case SDLK_e: {
                        keys[6] = 1;
                    }
                        break;

                    case SDLK_a: {
                        keys[7] = 1;
                    }
                        break;

                    case SDLK_s: {
                        keys[8] = 1;
                    }
                        break;

                    case SDLK_d: {
                        keys[9] = 1;
                    }
                        break;

                    case SDLK_z: {
                        keys[0xA] = 1;
                    }
                        break;

                    case SDLK_c: {
                        keys[0xB] = 1;
                    }
                        break;

                    case SDLK_4: {
                        keys[0xC] = 1;
                    }
                        break;

                    case SDLK_r: {
                        keys[0xD] = 1;
                    }
                        break;

                    case SDLK_f: {
                        keys[0xE] = 1;
                    }
                        break;

                    case SDLK_v: {
                        keys[0xF] = 1;
                    }
                        break;
                }
            }
                break;

            case SDL_KEYUP: {
                switch (event.key.keysym.sym) {
                    case SDLK_x: {
                        keys[0] = 0;
                    }
                        break;

                    case SDLK_1: {
                        keys[1] = 0;
                    }
                        break;

                    case SDLK_2: {
                        keys[2] = 0;
                    }
                        break;

                    case SDLK_3: {
                        keys[3] = 0;
                    }
                        break;

                    case SDLK_q: {
                        keys[4] = 0;
                    }
                        break;

                    case SDLK_w: {
                        keys[5] = 0;
                    }
                        break;

                    case SDLK_e: {
                        keys[6] = 0;
                    }
                        break;

                    case SDLK_a: {
                        keys[7] = 0;
                    }
                        break;

                    case SDLK_s: {
                        keys[8] = 0;
                    }
                        break;

                    case SDLK_d: {
                        keys[9] = 0;
                    }
                        break;

                    case SDLK_z: {
                        keys[0xA] = 0;
                    }
                        break;

                    case SDLK_c: {
                        keys[0xB] = 0;
                    }
                        break;

                    case SDLK_4: {
                        keys[0xC] = 0;
                    }
                        break;

                    case SDLK_r: {
                        keys[0xD] = 0;
                    }
                        break;

                    case SDLK_f: {
                        keys[0xE] = 0;
                    }
                        break;

                    case SDLK_v: {
                        keys[0xF] = 0;
                    }
                        break;
                }
            }
                break;
        }
    }
    shouldRun = !quit;
}

ChipEight::~ChipEight() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void ChipEight::updateScreen(const void *buffer, int pitch) const {
    if (!drawFlag) return;
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void ChipEight::setupScreen(const char *title, unsigned int scale) {
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(title, 100, 200, scale * VIDEO_WIDTH, scale * VIDEO_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, VIDEO_WIDTH,
                                VIDEO_HEIGHT);
}

inline void printUnimplemented(uint16_t a) {
    std::cout << "UNRECOGNISED OPCODE: " << std::hex << a << std::endl;
//    exit(-1);
}

void ChipEight::executeOpCode() {
    // Extract first byte
    uint16_t a = (opcode & 0xF000u);

    switch (a) {
        case 0x0000:
            if (opcode == 0x00E0) {
                OP_00E0();
                break;
            } else if (opcode == 0x00EE) {
                OP_00EE();
                break;
            } else {
                printUnimplemented(opcode);
                break;
            }
            break;
        case 0x1000:
            OP_1NNN();
            break;
        case 0x2000:
            OP_2NNN();
            break;
        case 0x3000:
            OP_3XKK();
            break;
        case 0x4000:
            OP_4XKK();
            break;
        case 0x5000:
            OP_5XY0();
            break;
        case 0x6000:
            OP_6XKK();
            break;
        case 0x7000:
            OP_7XKK();
            break;
        case 0x8000: {
            uint16_t lastNibble = opcode & 0x000Fu;
            switch (lastNibble) {
                case 0x0000:
                    OP_8XY0();
                    break;
                case 0x0001:
                    OP_8XY1();
                    break;
                case 0x0002:
                    OP_8XY2();
                    break;
                case 0x0003:
                    OP_8XY3();
                    break;
                case 0x0004:
                    OP_8XY4();
                    break;
                case 0x0005:
                    OP_8XY5();
                    break;
                case 0x0006:
                    OP_8XY6();
                    break;
                case 0x0007:
                    OP_8XY7();
                    break;
                case 0x000E:
                    OP_8XYE();
                    break;
                default:
                    printUnimplemented(opcode);
                    break;
            }
        }
            break;
        case 0x9000:
            OP_9XY0();
            break;
        case 0xA000:
            OP_ANNN();
            break;
        case 0xB000:
            OP_BNNN();
            break;
        case 0xC000:
            OP_CXKK();
            break;
        case 0xD000:
            OP_DXYN();
            break;
        case 0xE000: {
            uint16_t lastByte = opcode & 0x00FFu;
            if (lastByte == 0x009E) {
                OP_EX9E();
                break;
            } else if (lastByte == 0x00A1) {
                OP_EXA1();
                break;
            } else {
                printUnimplemented(opcode);
                break;
            }
        }
            break;
        case 0xF000: {
            uint16_t lastByte = opcode & 0x00FFu;
            switch (lastByte) {
                case 0x0007:
                    OP_FX07();
                    break;
                case 0x000A:
                    OP_FX0A();
                    break;
                case 0x0015:
                    OP_FX15();
                    break;
                case 0x0018:
                    OP_FX18();
                    break;
                case 0x001E:
                    OP_FX1E();
                    break;
                case 0x0029:
                    OP_FX29();
                    break;
                case 0x0033:
                    OP_FX33();
                    break;
                case 0x0055:
                    OP_FX55();
                    break;
                case 0x0065:
                    OP_FX65();
                    break;
                default:
                    printUnimplemented(opcode);
                    break;
            }
        }
            break;
        default:
            printUnimplemented(a);
            break;
    }
}

/**
 *   CLS - Clear the display
 */
void ChipEight::OP_00E0() {
    memset(video, 0, sizeof(video));
}

/**
 *   RET - Return from a subroutine
 */
void ChipEight::OP_00EE() {
    --sp;
    pc = stack[sp];
}

/**
 *   JMP - Jump to location NNN
 */
void ChipEight::OP_1NNN() {
    // Jump address is last 3 nibbles of opcode
    uint16_t address = opcode & 0x0FFFu;
    pc = address;
}

/**
 *   CALL - Call subroutine at NNN
 */
void ChipEight::OP_2NNN() {
    uint16_t address = opcode & 0x0FFFu;

    // Push current pc onto stack, and increment pointer
    stack[sp] = pc;
    ++sp;
    pc = address;
}

/**
 *   SE - Skip next instruction if Vx == kk
 */
void ChipEight::OP_3XKK() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = opcode & 0x00FFu;

    if (registers[x] == kk) {
        pc += 2;
    }
}

/**
 *   SNE - Skip next instruction if Vx != kk
 */
void ChipEight::OP_4XKK() {
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = opcode & 0x00FFu;

    if (registers[x] != kk) {
        pc += 2;
    }
}

void ChipEight::OP_5XY0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] == registers[Vy]) {
        pc += 2;
    }
}

void ChipEight::OP_6XKK() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = opcode & 0x00FFu;
    registers[Vx] = kk;
}

void ChipEight::OP_7XKK() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = opcode & 0x00FFu;

    registers[Vx] += kk;
}

void ChipEight::OP_8XY0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vy];
}

void ChipEight::OP_8XY1() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] |= registers[Vy];
}

void ChipEight::OP_8XY2() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] &= registers[Vy];
}

void ChipEight::OP_8XY3() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] ^= registers[Vy];
}

void ChipEight::OP_8XY4() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint16_t result = registers[Vx] + registers[Vy];
    if (result > 255) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }
    registers[Vx] = result & 0xFFu;
}

void ChipEight::OP_8XY5() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] > registers[Vy]) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }

    registers[Vx] -= registers[Vy];
}

void ChipEight::OP_8XY6() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    // Save LSB in VF
    registers[0xF] = (registers[Vx] & 0x1u);

    registers[Vx] >>= 1u;
}

void ChipEight::OP_8XY7() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vy] > registers[Vx]) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }

    registers[Vx] = registers[Vy] - registers[Vx];
}

void ChipEight::OP_8XYE() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    registers[0xF] = (registers[Vx] & 0x80u) >> 7u;
    registers[Vx] <<= 1u;
}

void ChipEight::OP_9XY0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] != registers[Vy]) {
        pc += 2;
    }
}

void ChipEight::OP_ANNN() {
    uint16_t address = opcode & 0x0FFFu;
    indexRegister = address;
}

void ChipEight::OP_BNNN() {
    uint16_t nnn = opcode & 0x0FFFu;
    pc = registers[0] + nnn;
}

void ChipEight::OP_CXKK() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = opcode & 0x00FFu;

    registers[Vx] = randByte(randGen) & kk;
}

void ChipEight::OP_DXYN() {

    // Extract Vx, Vy, n (height)
    uint8_t height = (opcode & 0x000Fu);
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    // Wrap pixels around if they overflow off screen
    uint8_t x = registers[Vx] % VIDEO_WIDTH;
    uint8_t y = registers[Vy] % VIDEO_HEIGHT;

    // Set VF register to 0
    registers[0xF] = 0;

    for (unsigned int row = 0; row < height; ++row) {
        uint8_t spriteByte = memory[indexRegister + row];

        for (unsigned int col = 0; col < 8; ++col) {
            // Get each pixel by shifting (0x80 = 1000 0000)
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint32_t *screenPixel = &video[(y + row) * VIDEO_WIDTH + (x + col)];

            // Sprite pixel is on
            if (spritePixel) {
                // Screen pixel also on - collision
                if (*screenPixel == 0xFFFFFFFF) {
                    registers[0xF] = 1;
                }

                // Effectively XOR with the sprite pixel
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }

    drawFlag = true;
}

void ChipEight::OP_EX9E() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    if (keys[registers[Vx]] == 1) {
        pc += 2;
    }
}

void ChipEight::OP_EXA1() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    if (keys[registers[Vx]] == 0) {
        pc += 2;
    }
}

void ChipEight::OP_FX07() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    registers[Vx] = delayRegister;
}

void ChipEight::OP_FX0A() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    bool keyPressed = false;

    for (int i = 0; i <= 15; i++) {
        if (keys[i]) {
            keyPressed = true;
            break;
        }
    }

    if (!keyPressed)
        pc -= 2;
}

void ChipEight::OP_FX15() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    delayRegister = registers[Vx];
}

void ChipEight::OP_FX18() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    soundRegister = registers[Vx];
}

void ChipEight::OP_FX1E() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    indexRegister += registers[Vx];
}

void ChipEight::OP_FX29() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t digit = registers[Vx];

    indexRegister = FONT_START_ADDRESS + (5 * digit);
}

void ChipEight::OP_FX33() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t value = registers[Vx];

    // Ones-place
    writeToMemory(indexRegister + 2, value % 10);
    value /= 10;

    // Tens-place
    writeToMemory(indexRegister + 1, value % 10);
    value /= 10;

    // Hundreds-place
    writeToMemory(indexRegister, value % 10);
}

void ChipEight::OP_FX55() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for (int i = 0; i <= Vx; i++) {
        writeToMemory(indexRegister + i, registers[i]);
    }
}

void ChipEight::OP_FX65() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for (int i = 0; i <= Vx; i++) {
        registers[i] = memory[indexRegister + i];
    }
}

void ChipEight::writeToMemory(int index, uint8_t value) {
    if (index > START_ADDRESS) {
        memory[index] = value;
    } else {
        std::cout << "TRIED TO WRITE TO ROM" << std::endl;
    }
}

