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
    std::cout << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << instruction << std::endl;

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
        break;
    case 0x1:
        pc = NNN;
        break;
    case 0x2:
        break;
    case 0x3:
        break;
    case 0x4:
        break;
    case 0x5:
        break;
    case 0x6:
        reg[X] = NN;
        break;
    case 0x7:
        reg[X] = reg[X] + NN;
        break;
    case 0x8:
        break;
    case 0x9:
        break;
    case 0xA:
        indexReg = NNN;
        break;
    case 0xB:
        break;
    case 0xC:
        break;
    case 0xD:
    {
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
    }
    break;
    case 0xE:
        break;
    case 0xF:
        break;

    default:
        std::cerr << "Unknown opcode: "
                  << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << instruction
                  << std::nouppercase << std::dec << std::setfill(' ') << std::endl;
    }
}
