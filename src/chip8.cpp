#include "chip8.h"
#include <iostream>
#include <fstream>
#include <iomanip>

constexpr std::array<uint8_t, 80> FONT = {
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

Chip8::Chip8()
{
    pc = PROGRAM_START;
    // Puts the font information in the memory of the emulator
    for (size_t i = 0; i < FONT.size(); i++)
    {
        memory[FONT_START + i] = FONT[i];
    }
}

Chip8::~Chip8()
{
}

const std::array<bool, Chip8::DISPLAY_WIDTH * Chip8::DISPLAY_HEIGHT> &Chip8::getDisplay() const
{
    return display;
}

bool Chip8::loadRom(const std::string &path)
{

    // Opens the file based on th filepath
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    // checks if the file is open or not before continuing
    if (!file.is_open())
    {
        std::cerr << "Error: Unable to open file!" << std::endl;
        return false;
    }

    // gets the size of the rom file so that it can be checked against MAX_ROM_SIZE
    std::streamsize romSize = file.tellg();

    // This is just in case since I'm comparing signed and unsigned values
    if (romSize < 0)
    {
        std::cerr << "Couldnt read file size" << std::endl;
        return false;
    }

    // brings file cursor back to the start of the file so it can start reading
    file.seekg(0, std::ios::beg);

    // checks that the rom is not too large
    if (MAX_ROM_SIZE < romSize)
    {
        std::cerr << "Error: ROM size too large!" << std::endl;
        return false;
    }

    // CODE ONLY GETS HERE AFTER PASSING ALL OF THE EARLY RETRUNS ABOVE
    // reads the file starting from the adress of memory(PROGRAM_START)
    // it also casts the address of the array from uint8_t* to char* for the sake of the read method
    file.read(reinterpret_cast<char *>(&memory[PROGRAM_START]), romSize);
    return true;
}

void Chip8::printState() const
{
    std::cout << std::hex << std::uppercase << std::setfill('0');

    // Program counter
    std::cout << "PC: 0x" << std::setw(3) << pc << '\n';

    // First character of the font (should be F0 90 90 90 F0)
    std::cout << "Font @ 0x" << std::setw(3) << FONT_START << ": ";
    for (int i = 0; i < 5; ++i)
        std::cout << std::setw(2) << static_cast<int>(memory[FONT_START + i]) << ' ';
    std::cout << '\n';

    // First 16 bytes of the loaded ROM
    std::cout << "ROM  @ 0x" << std::setw(3) << PROGRAM_START << ": ";
    for (int i = 0; i < 16; ++i)
        std::cout << std::setw(2) << static_cast<int>(memory[PROGRAM_START + i]) << ' ';
    std::cout << '\n';

    // Put cout back to normal decimal output
    std::cout << std::dec << std::nouppercase << std::setfill(' ');
}

void Chip8::step()
{
    // One step goes fetch --> decode --> execute

    // Fetching the opcodes to make the instructions
    uint16_t instruction = (memory[pc] << 8) | memory[pc + 1];

    // prints the instruction with formatting of hex number
    // std::cout << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << instruction << std::endl;

    // Resets the output formatting to normal decimal output
    std::cout << std::dec << std::nouppercase << std::setfill(' ');
    pc += 2;

    // Decode part of the step method
    uint8_t firstDigit = (instruction >> 12) & 0xF;
    uint8_t X = (instruction >> 8) & 0xF; // Second
    uint8_t Y = (instruction >> 4) & 0xF; // Third
    uint8_t N = (instruction) & 0xF;      // fourth
    uint16_t NN = (instruction) & 0xFF;   // last two
    uint16_t NNN = (instruction) & 0xFFF; // last three

    // execute part of the step method
    switch (firstDigit)
    {
    case 0x0:
        if (NNN == 0x0E0)
        {
            // 0x00E0 Instruction: Clears screen
            display.fill(false);
        }
        else if (NNN == 0x0EE)
        {
            pc = pop();
        }
        break;
    case 0x1:
        pc = NNN;
        break;
    case 0x2:
        push(pc);
        pc = NNN;
        break;
    case 0x3:
        if (reg[X] == NN)
        {
            pc += 2;
        }
        break;
    case 0x4:
        if (reg[X] != NN)
        {
            pc += 2;
        }
        break;
    case 0x5:
        if (reg[X] == reg[Y])
        {
            pc += 2;
        }
        break;
    case 0x6:
        reg[X] = NN;
        break;
    case 0x7:
        reg[X] = reg[X] + NN;
        break;
    case 0x8:
        switch (N)
        {
        case 0x0:
            reg[X] = reg[Y];
            break;
        case 0x1:
            reg[X] = reg[X] | reg[Y];
            reg[0xF] = 0;
            break;
        case 0x2:
            reg[X] = reg[X] & reg[Y];
            reg[0xF] = 0;
            break;
        case 0x3:
            reg[X] = reg[X] ^ reg[Y];
            reg[0xF] = 0;
            break;
        case 0x4:
        {
            int temp = reg[X] + reg[Y];
            reg[X] = temp;
            if (temp > 255)
            {
                reg[0xF] = 1;
            }
            else
            {
                reg[0xF] = 0;
            }
        }
        break;
        case 0x5:
        {
            int temp = reg[X] - reg[Y];
            reg[X] = temp;
            if (temp >= 0)
            {
                reg[0xF] = 1;
            }
            else
            {
                reg[0xF] = 0;
            }
        }
        break;
        case 0x6:
        {
            reg[X] = reg[Y];
            uint8_t temp = reg[X] & 0b1;
            reg[X] = reg[X] >> 1;
            reg[0xF] = temp;
        }
        break;
        case 0x7:
        {
            int temp = reg[Y] - reg[X];
            reg[X] = temp;
            if (temp >= 0)
            {
                reg[0xF] = 1;
            }
            else
            {
                reg[0xF] = 0;
            }
        }
        break;
        case 0xE:
        {
            reg[X] = reg[Y];
            uint8_t temp = (reg[X] >> 7) & 0b1;
            reg[X] = reg[X] << 1;
            reg[0xF] = temp;
        }
        break;
        default:
            std::cerr << "Unknown opcode: "
                      << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << instruction
                      << std::nouppercase << std::dec << std::setfill(' ') << std::endl;
            break;
        }
        break;
    case 0x9:
        if (reg[X] != reg[Y])
        {
            pc += 2;
        }
        break;
    case 0xA:
        indexReg = NNN;
        break;
    case 0xB:
        pc = NNN + reg[0];
        break;
    case 0xC:
    {
        std::uniform_int_distribution<int> distribution(0, 255);
        int randomNumber = distribution(rng);
        reg[X] = randomNumber & NN;
    }
    break;
    case 0xD:
    {
        // Display wait: only one draw per frame
        if (drawnThisFrame)
        {
            pc -= 2;
            break;
        }
        int xcoord = reg[X] % DISPLAY_WIDTH;
        int ycoord = reg[Y] % DISPLAY_HEIGHT;
        reg[0xF] = 0;

        uint8_t pixelData;
        int coulumnCount;

        // repeating this N times
        for (uint8_t i = 0; i < N; i++)
        {
            // For each row
            coulumnCount = 0;
            // Moves doesn to the next row after filling the current row
            pixelData = memory[indexReg + i];

            // Filling the row with the pixel data.
            for (int j = 7; j >= 0; j--)
            {
                if ((xcoord + coulumnCount >= DISPLAY_WIDTH || (ycoord + i) >= DISPLAY_HEIGHT))
                {
                    coulumnCount += 1;
                    continue;
                }
                if ((pixelData >> j) & 0b1)
                {
                    if (display[(xcoord + coulumnCount) + (DISPLAY_WIDTH * (ycoord + i))])
                    {
                        display[(xcoord + coulumnCount) + (DISPLAY_WIDTH * (ycoord + i))] = false;
                        reg[0xF] = 1;
                    }
                    else
                    {
                        display[(xcoord + coulumnCount) + (DISPLAY_WIDTH * (ycoord + i))] = true;
                    }
                }
                coulumnCount += 1;
            }
        }
        drawnThisFrame = true;
    }
    break;
    case 0xE:
    {
        switch (NN)
        {
        case 0x9E:
            if (keys[reg[X] & 0xF])
            {
                pc += 2;
            }
            break;
        case 0xA1:
            if (!keys[reg[X] & 0xF])
            {
                pc += 2;
            }
            break;
        default:
            std::cerr << "Unknown opcode: "
                      << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << instruction
                      << std::nouppercase << std::dec << std::setfill(' ') << std::endl;
        }
    }
    break;
    case 0xF:
        switch (NN)
        {
        case 0x07:
            reg[X] = delayTimer;
            break;

        case 0x15:
            delayTimer = reg[X];
            break;

        case 0x18:
            soundTimer = reg[X];
            break;

        case 0x1E:
            indexReg += reg[X];
            break;

        case 0x0A:
        {
            if (waitingKey == -1)
            {
                for (int k = 0; k < NUM_KEYS; k++)
                {
                    if (keys[k])
                    {
                        waitingKey = k;
                        break;
                    }
                }
                pc -= 2;
            }
            else
            {
                if (!keys[waitingKey])
                {
                    reg[X] = waitingKey;
                    waitingKey = -1;
                }
                else
                {
                    pc -= 2;
                }
            }
        }
        break;

        case 0x29:
        {
            int temp = reg[X] & 0xF;
            indexReg = FONT_START + (5 * temp);
        }
        break;

        case 0x33:
            // gets the hundreds, tens and ones digits from reg[X]
            memory[indexReg] = reg[X] / 100;
            memory[indexReg + 1] = (reg[X] / 10) % 10;
            memory[indexReg + 2] = reg[X] % 10;
            break;

        case 0x55:
        {
            for (int i = 0; i <= X; i++)
            {
                memory[indexReg + i] = reg[i];
            }
            indexReg += X + 1;
        }
        break;

        case 0x65:
        {
            for (int i = 0; i <= X; i++)
            {
                reg[i] = memory[indexReg + i];
            }
            indexReg += X + 1;
        }
        break;
        default:
            std::cerr << "Unknown opcode: "
                      << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << instruction
                      << std::nouppercase << std::dec << std::setfill(' ') << std::endl;
        }
        break;

    default:
        std::cerr << "Unknown opcode: "
                  << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << instruction
                  << std::nouppercase << std::dec << std::setfill(' ') << std::endl;
    }
}

void Chip8::push(uint16_t value)
{
    // logic for pushing into stack
    if (stackPtr < STACK_SIZE)
    {
        stack[stackPtr] = value;
        stackPtr += 1;
    }
    else
    {
        std::cerr << "STACK IS FULL! " << std::endl;
    }
}

uint16_t Chip8::pop()
{
    // logic for popping value out of stack
    if (stackPtr == 0)
    {
        std::cerr << "STACK IS EMPTY! " << std::endl;
        return 0;
    }
    stackPtr -= 1;
    uint16_t temp = stack[stackPtr];
    stack[stackPtr] = 0;
    return temp;
}

void Chip8::tickTimers()
{

    if (delayTimer > 0)
    {
        delayTimer -= 1;
    }

    if (soundTimer > 0)
    {
        soundTimer -= 1;
    }
}

void Chip8::setKey(uint8_t key, bool pressed)
{
    keys[key] = pressed;
}

uint8_t Chip8::getSoundTimer() const
{
    return soundTimer;
}

void Chip8::newFrame()
{
    drawnThisFrame = false;
}