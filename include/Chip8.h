#pragma once
#include <stdint.h>
#include <array>

class Chip8
{
private:
    std::array<uint8_t, 4096> memory{};
    std::array<uint8_t, 16> reg{};
    uint32_t indexReg{};
    uint32_t pc{};
    uint32_t delayTimer{};
    uint8_t soundTimer{};
    std::array<uint16_t, 16> stack{};
    uint16_t *stackPtr = NULL;

public:
    Chip8();
    ~Chip8();
};

Chip8::Chip8()
{
}

Chip8::~Chip8()
{
}