#pragma once
#include <stdint.h>
#include <array>
#include <string>
#include <random>

class Chip8
{
public:
    Chip8();
    ~Chip8();
    static constexpr int DISPLAY_WIDTH = 64;
    static constexpr int DISPLAY_HEIGHT = 32;
    static constexpr int MEMORY_SIZE = 4096;
    static constexpr int NUM_REGISTERS = 16;
    static constexpr int STACK_SIZE = 16;
    static constexpr int NUM_KEYS = 16;
    static constexpr uint16_t PROGRAM_START = 0x200;
    static constexpr uint16_t FONT_START = 0x050;
    static constexpr uint16_t MAX_ROM_SIZE = MEMORY_SIZE - PROGRAM_START;

    const std::array<bool, DISPLAY_WIDTH * DISPLAY_HEIGHT> &getDisplay() const;

    bool loadRom(const std::string &path);
    void printState() const;
    void step();
    void push(uint16_t value);
    uint16_t pop();
    void tickTimers();
    void setKey(uint8_t key, bool pressed);

private:
    std::array<uint8_t, MEMORY_SIZE> memory{};
    std::array<uint8_t, NUM_REGISTERS> reg{};
    uint16_t indexReg{};
    uint16_t pc{};
    uint8_t delayTimer{};
    uint8_t soundTimer{};
    std::array<uint16_t, STACK_SIZE> stack{};
    uint8_t stackPtr{};
    std::mt19937 rng{std::random_device{}()};

    std::array<bool, DISPLAY_WIDTH * DISPLAY_HEIGHT> display{};
    std::array<bool, NUM_KEYS> keys{};
};
