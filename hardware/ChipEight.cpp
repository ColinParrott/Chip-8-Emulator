#include "ChipEight.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <windows.h>
#include <chrono>


/**
 * Initialise Chip-8
 */
ChipEight::ChipEight(bool loadStoreQuirk, bool shiftQuirk)
        : randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    // Set all vars to initial values
    opcode = -1;
    memset(registers, 0, sizeof(registers));
    indexRegister = 0;
    pc = START_ADDRESS;
    sp = 0;
    delayRegister = 0;
    soundRegister = 0;
    memset(keypad, 0, sizeof(keypad));
    memset(stack, 0, sizeof(stack));
    memset(memory, 0, sizeof(memory));
    shouldRun = true;
    drawFlag = false;
    this->loadStoreQuirk = loadStoreQuirk;
    this->shiftQuirk = shiftQuirk;

    // Load font set into memory 0x00 - 0x50 (0 to 80)
    for (int i = 0; i < FONT_SET_SIZE; i++)
    {
        memory[FONT_START_ADDRESS + i] = fontset[i];
    }

    // Initialize RNG
    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
}

/**
 * Loads Chip-8 ROM from a file into memory
 * @param path Path to file
 */
void ChipEight::LoadROM(const char *path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (file.is_open())
    {
        // Get size of file and allocate a buffer to hold the contents
        std::streampos size = file.tellg();
        char *buffer = new char[size];

        // Go back to the beginning of the file and fill the buffer
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        // Load the ROM contents into the Chip8's memory, starting at 0x200
        for (auto i = 0; i < size; ++i)
        {
            memory[START_ADDRESS + i] = buffer[i];
        }

        // Free the buffer
        delete[] buffer;
    }
}

/**
 * Should be called each cycle to execute opcode and update delay & sound registers
 */
void ChipEight::executeCycle()
{
    for (int i = 0; i < 8; i++)
    {
        // Opcode is 2 bytes long, so merge two successive bytes
        // Extend first byte to 16 bits (by shifting left 8 which pads 8 zeroes effectively), then
        // OR with next byte to replace padded zeroes with the second byte's value
        opcode = (memory[pc] << 8u) | memory[pc + 1];

        // Pre-emptively add 2 to PC, to move to next opcode (executed opcode may overwrite this)
        pc += 2;

        executeOpCode();
    }

    if (delayRegister > 0)
    {
        --delayRegister;
    }

    if (soundRegister > 0)
    {
        --soundRegister;

        if (soundRegister > 0)
        {
            Beep(500, 16);
        }
    }
}

/**
 * Handle input using SDL, and update Chip-8's keypad when keys are pressed/released
 */
void ChipEight::processInputs()
{
    bool quit = false;

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
            {
                quit = true;
            }
                break;

            case SDL_KEYDOWN:
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                    {
                        quit = true;
                    }
                        break;

                    case SDLK_x:
                    {
                        keypad[0] = 1;
                    }
                        break;

                    case SDLK_1:
                    {
                        keypad[1] = 1;
                    }
                        break;

                    case SDLK_2:
                    {
                        keypad[2] = 1;
                    }
                        break;

                    case SDLK_3:
                    {
                        keypad[3] = 1;
                    }
                        break;

                    case SDLK_q:
                    {
                        keypad[4] = 1;
                    }
                        break;

                    case SDLK_w:
                    {
                        keypad[5] = 1;
                    }
                        break;

                    case SDLK_e:
                    {
                        keypad[6] = 1;
                    }
                        break;

                    case SDLK_a:
                    {
                        keypad[7] = 1;
                    }
                        break;

                    case SDLK_s:
                    {
                        keypad[8] = 1;
                    }
                        break;

                    case SDLK_d:
                    {
                        keypad[9] = 1;
                    }
                        break;

                    case SDLK_z:
                    {
                        keypad[0xA] = 1;
                    }
                        break;

                    case SDLK_c:
                    {
                        keypad[0xB] = 1;
                    }
                        break;

                    case SDLK_4:
                    {
                        keypad[0xC] = 1;
                    }
                        break;

                    case SDLK_r:
                    {
                        keypad[0xD] = 1;
                    }
                        break;

                    case SDLK_f:
                    {
                        keypad[0xE] = 1;
                    }
                        break;

                    case SDLK_v:
                    {
                        keypad[0xF] = 1;
                    }
                        break;
                }
            }
                break;

            case SDL_KEYUP:
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_x:
                    {
                        keypad[0] = 0;
                    }
                        break;

                    case SDLK_1:
                    {
                        keypad[1] = 0;
                    }
                        break;

                    case SDLK_2:
                    {
                        keypad[2] = 0;
                    }
                        break;

                    case SDLK_3:
                    {
                        keypad[3] = 0;
                    }
                        break;

                    case SDLK_q:
                    {
                        keypad[4] = 0;
                    }
                        break;

                    case SDLK_w:
                    {
                        keypad[5] = 0;
                    }
                        break;

                    case SDLK_e:
                    {
                        keypad[6] = 0;
                    }
                        break;

                    case SDLK_a:
                    {
                        keypad[7] = 0;
                    }
                        break;

                    case SDLK_s:
                    {
                        keypad[8] = 0;
                    }
                        break;

                    case SDLK_d:
                    {
                        keypad[9] = 0;
                    }
                        break;

                    case SDLK_z:
                    {
                        keypad[0xA] = 0;
                    }
                        break;

                    case SDLK_c:
                    {
                        keypad[0xB] = 0;
                    }
                        break;

                    case SDLK_4:
                    {
                        keypad[0xC] = 0;
                    }
                        break;

                    case SDLK_r:
                    {
                        keypad[0xD] = 0;
                    }
                        break;

                    case SDLK_f:
                    {
                        keypad[0xE] = 0;
                    }
                        break;

                    case SDLK_v:
                    {
                        keypad[0xF] = 0;
                    }
                        break;
                }
            }
                break;
        }
    }
    shouldRun = !quit;
}

/**
 * Clean up all SDL stuff
 */
ChipEight::~ChipEight()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

/**
 * Updates the screen and should be called every clock cycle
 * @param buffer Array of pixels
 * @param pitch Pitch used by SDL
 */
void ChipEight::updateScreen(const void *buffer, int pitch)
{
    if (!drawFlag)
    {
        return;
    }

    SDL_UpdateTexture(texture, nullptr, buffer, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
    drawFlag = false;
}

/**
 * Sets up the SDL window
 *
 * @param title Title of the window
 * @param scale Scaling factor for the graphics
 */
void ChipEight::setupScreen(const char *title, unsigned int scale)
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(title, 100, 200, scale * VIDEO_WIDTH, scale * VIDEO_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, VIDEO_WIDTH, VIDEO_HEIGHT);
}

/**
 * Call this if we find an opcode that we don't recognise
 *
 * @param a Opcode that wasn't recognised
 */
inline void printUnimplemented(uint16_t a)
{
    std::cout << "UNRECOGNISED OPCODE: " << std::hex << a << std::endl;
}

/**
 * Analyses the opcode and calls the relevant opcode method
 */
void ChipEight::executeOpCode()
{
    // Extract first byte
    uint16_t a = (opcode & 0xF000u);

    switch (a)
    {
        case 0x0000:
            if (opcode == 0x00E0)
            {
                OP_00E0();
            }
            else if (opcode == 0x00EE)
            {
                OP_00EE();
            }
            else
            {
                printUnimplemented(opcode);
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
        case 0x8000:
        {
            uint16_t lastNibble = opcode & 0x000Fu;
            switch (lastNibble)
            {
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
        case 0xE000:
        {
            uint16_t lastByte = opcode & 0x00FFu;
            if (lastByte == 0x009E)
            {
                OP_EX9E();
            }
            else if (lastByte == 0x00A1)
            {
                OP_EXA1();
            }
            else
            {
                printUnimplemented(opcode);
            }
        }
            break;
        case 0xF000:
        {
            uint16_t lastByte = opcode & 0x00FFu;
            switch (lastByte)
            {
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
void ChipEight::OP_00E0()
{
    memset(video, 0, sizeof(video));
}

/**
 *   RET - Return from a subroutine
 */
void ChipEight::OP_00EE()
{
    --sp;
    pc = stack[sp];
}

/**
 *   JMP - Jump to location NNN
 */
void ChipEight::OP_1NNN()
{
    // Jump address is last 3 nibbles of opcode
    uint16_t address = opcode & 0x0FFFu;
    pc = address;
}

/**
 *   CALL - Call subroutine at NNN
 */
void ChipEight::OP_2NNN()
{
    uint16_t address = opcode & 0x0FFFu;

    // Push current pc onto stack, and increment pointer
    stack[sp] = pc;
    ++sp;
    pc = address;
}

/**
 *   SE - Skip next instruction if Vx == kk
 */
void ChipEight::OP_3XKK()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = opcode & 0x00FFu;

    if (registers[x] == kk)
    {
        pc += 2;
    }
}

/**
 *   SNE Vx, kk - Skip next instruction if Vx != kk
 */
void ChipEight::OP_4XKK()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = opcode & 0x00FFu;

    if (registers[x] != kk)
    {
        pc += 2;
    }
}

/**
 *   SE Vx, Vy - Skip next instruction if Vx == kk
 */
void ChipEight::OP_5XY0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] == registers[Vy])
    {
        pc += 2;
    }
}


/**
 *   LD Vx, kk - Set Vx = kk
 */
void ChipEight::OP_6XKK()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = opcode & 0x00FFu;
    registers[Vx] = kk;
}

/**
 *   ADD Vx, kk - Set Vx = Vx + kk
 */
void ChipEight::OP_7XKK()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = opcode & 0x00FFu;

    registers[Vx] += kk;
}

/**
 *   LD Vx, Vy - Set Vx = Vy
 */
void ChipEight::OP_8XY0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vy];
}

/**
 *   OR Vx, kk - Set Vx = Vx OR Vy
 */
void ChipEight::OP_8XY1()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] |= registers[Vy];
}

/**
 *   AND Vx, kk - Set Vx = Vx AND Vy
 */
void ChipEight::OP_8XY2()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] &= registers[Vy];
}

/**
 *   XOR Vx, kk - Set Vx = Vx XOR Vy
 */
void ChipEight::OP_8XY3()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] ^= registers[Vy];
}

/**
 *   ADD Vx, Vy  - Set Vx = Vx + Vy, set VF = carry
 */
void ChipEight::OP_8XY4()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint16_t result = registers[Vx] + registers[Vy];
    if (result > 255)
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }
    registers[Vx] = result & 0xFFu;
}

/**
 *   SUB Vx, Vy  - Set Vx = Vx - Vy, set VF = NOT borrow
 */
void ChipEight::OP_8XY5()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] > registers[Vy])
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }

    registers[Vx] -= registers[Vy];
}

/**
 *   SHR Vx - If LSB of Vx is 1 then set VF = 1 otherwise 0, then set Vx = Vx >> 1
 */
void ChipEight::OP_8XY6()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (shiftQuirk)
    {
        Vy = Vx;
    }

    // Save LSB in VF
    registers[0xF] = (registers[Vy] & 0x1u);
    registers[Vx] = registers[Vy] >> 1u;
}

/**
 *   SUBN Vx, Vy - Set Vx = Vy - Vx, set VF = NOT borrow
 */
void ChipEight::OP_8XY7()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vy] > registers[Vx])
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }

    registers[Vx] = registers[Vy] - registers[Vx];
}

/**
 *   SHR Vx - If MSB of Vx is 1 then set VF = 1 otherwise 0, then set Vx = Vx >> 1
 */
void ChipEight::OP_8XYE()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (shiftQuirk)
    {
        Vy = Vx;
    }

    registers[0xF] = (registers[Vy] & 0x80u) >> 7u;
    registers[Vx] = registers[Vy] << 1u;
}

/**
 *   SNE Vx, Vy - Skip next instruction if Vx != Vy
 */
void ChipEight::OP_9XY0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] != registers[Vy])
    {
        pc += 2;
    }
}

/**
 *   LD I, nnn - Set I = nnn
 */
void ChipEight::OP_ANNN()
{
    uint16_t address = opcode & 0x0FFFu;
    indexRegister = address;
}

/**
 *   JP VO, nnn - Jump to location nnn + V0
 */
void ChipEight::OP_BNNN()
{
    uint16_t nnn = opcode & 0x0FFFu;
    pc = registers[0] + nnn;
}

/**
 *  RND Vx, kk - Set Vx = random byte AND kk
 */
void ChipEight::OP_CXKK()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = opcode & 0x00FFu;

    registers[Vx] = randByte(randGen) & kk;
}

/**
 *  DRW Vx, Vy, n - Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
 */
void ChipEight::OP_DXYN()
{
    // Extract Vx, Vy, n (height)
    uint8_t height = (opcode & 0x000Fu);
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    // Extract x & y from registers
    uint8_t x = registers[Vx];
    uint8_t y = registers[Vy];

    // Set VF register to 0
    registers[0xF] = 0;

    for (unsigned int row = 0; row < height; ++row)
    {
        uint8_t spriteByte = memory[indexRegister + row];

        for (unsigned int col = 0; col < 8; ++col)
        {
            // Get each pixel by shifting (0x80 = 1000 0000)
            uint8_t spritePixel = spriteByte & (0x80u >> col);

            // Get pixel index in 1D array using: X + (Y * WIDTH) and modulo to wrap around x,y coords
            // - without wrapping index goes outside video array
            uint32_t *screenPixel = &video[((x + col) % VIDEO_WIDTH) + (((y + row) % VIDEO_HEIGHT) * VIDEO_WIDTH)];

            // Sprite pixel is on
            if (spritePixel)
            {
                // Screen pixel also on - collision
                if (*screenPixel == 0xFFFFFFFF)
                {
                    registers[0xF] = 1;
                }

                // Effectively XOR with the sprite pixel
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }

    drawFlag = true;
}

/**
 *  SKP Vx - Skip next instruction if key with value of Vx is pressed
 */
void ChipEight::OP_EX9E()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    if (keypad[registers[Vx]] == 1)
    {
        pc += 2;
    }
}

/**
 *  SKNP Vx - Skip next instruction if key with value of Vx is NOT pressed
 */
void ChipEight::OP_EXA1()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    if (keypad[registers[Vx]] == 0)
    {
        pc += 2;
    }
}

/**
 *  LD Vx, DT - Set Vx = delay register value
 */
void ChipEight::OP_FX07()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    registers[Vx] = delayRegister;
}

/**
 *  LD Vx, K - Wait for a key press, store the value of key in Vx
 */
void ChipEight::OP_FX0A()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    if (keypad[0])
    {
        registers[Vx] = 0;
    }
    else if (keypad[1])
    {
        registers[Vx] = 1;
    }
    else if (keypad[2])
    {
        registers[Vx] = 2;
    }
    else if (keypad[3])
    {
        registers[Vx] = 3;
    }
    else if (keypad[4])
    {
        registers[Vx] = 4;
    }
    else if (keypad[5])
    {
        registers[Vx] = 5;
    }
    else if (keypad[6])
    {
        registers[Vx] = 6;
    }
    else if (keypad[7])
    {
        registers[Vx] = 7;
    }
    else if (keypad[8])
    {
        registers[Vx] = 8;
    }
    else if (keypad[9])
    {
        registers[Vx] = 9;
    }
    else if (keypad[10])
    {
        registers[Vx] = 10;
    }
    else if (keypad[11])
    {
        registers[Vx] = 11;
    }
    else if (keypad[12])
    {
        registers[Vx] = 12;
    }
    else if (keypad[13])
    {
        registers[Vx] = 13;
    }
    else if (keypad[14])
    {
        registers[Vx] = 14;
    }
    else if (keypad[15])
    {
        registers[Vx] = 15;
    }
    else
    {
        pc -= 2;
    }

//    bool keyPressed = false;
//
//    for (int i = 0; i <= 15; i++)
//    {
//        if (keypad[i])
//        {
//            keyPressed = true;
//            registers[Vx] = i;
//            break;
//        }
//    }
//
//    if (!keyPressed)
//    {
//        pc -= 2;
//    }
}

/**
 *  LD DT, Vx - Set delay register = Vx
 */
void ChipEight::OP_FX15()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    delayRegister = registers[Vx];
}

/**
 *  LD ST, Vx - Set sound register = Vx
 */
void ChipEight::OP_FX18()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    soundRegister = registers[Vx];
}

/**
 *  ADD, I, Vx - Set I = I + Vx
 */
void ChipEight::OP_FX1E()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    indexRegister += registers[Vx];
}

/**
 *  LD F, Vx - Set I = location of sprite for digit Vx
 */
void ChipEight::OP_FX29()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t digit = registers[Vx];

    indexRegister = FONT_START_ADDRESS + (5 * digit);
}

/**
 *  LD B, Vx - Store BCD representation of Vx in memory locations I, I+1, and I+2
 */
void ChipEight::OP_FX33()
{
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

/**
 *  LD [I], Vx - Copy the values of registers V0 through Vx into memory, starting at the address in I.
 */
void ChipEight::OP_FX55()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for (int i = 0; i <= Vx; i++)
    {
        writeToMemory(indexRegister + i, registers[i]);
    }

    if (!loadStoreQuirk)
    {
        indexRegister += Vx + 1;
    }
}

/**
 *  LD Vx, [I] - Copy the values from memory starting at location I into registers V0 through Vx.
 */
void ChipEight::OP_FX65()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for (int i = 0; i <= Vx; i++)
    {
        registers[i] = memory[indexRegister + i];
    }

    if (!loadStoreQuirk)
    {
        indexRegister += Vx + 1;
    }
}

/**
 * Function for writing to values into memory
 * @param index Index of byte to replace
 * @param value Value to put into that byte
 */
void ChipEight::writeToMemory(int index, uint8_t value)
{
    // Ensure we're not writing inside the ROM area (0x000 - 0x200)
    if (index > START_ADDRESS)
    {
        memory[index] = value;
    }
    else
    {
        std::cout << "TRIED TO WRITE TO ROM" << std::endl;
    }
}

