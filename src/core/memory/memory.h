#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>

constexpr uint32_t MEM_SIZE = 1024 * 1024; // 1 MB (RAM simples por enquanto)

struct Memory {
public:
    uint8_t data[MEM_SIZE];

    Memory() {
        reset();
    }

    void reset() {
        std::memset(data, 0, sizeof(data));
    }

    uint8_t read8(uint32_t addr) const;
    uint16_t read16(uint32_t addr) const;
    uint32_t read32(uint32_t addr) const;

    void write8(uint32_t addr, uint8_t value);
    void write16(uint32_t addr, uint16_t value);
    void write32(uint32_t addr, uint32_t value);
};
