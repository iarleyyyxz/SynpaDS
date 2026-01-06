#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>

constexpr uint32_t BIOS_SIZE = 32 * 1024;
constexpr uint32_t RAM_SIZE = 4 * 1024 * 1024;
constexpr uint32_t WRAM_SIZE = 64 * 1024;
constexpr uint32_t ROM_MAX = 64 * 1024 * 1024;

struct Memory {

    // ----------------------------
    // REGIÕES
    // ----------------------------
    uint8_t bios[BIOS_SIZE];
    uint8_t ram[RAM_SIZE];
    uint8_t wram[WRAM_SIZE];
    uint8_t rom[ROM_MAX];
    uint32_t romSize = 0;

    Memory() {
        reset();
    }

    void reset() {
        memset(bios, 0, sizeof(bios));
        memset(ram, 0, sizeof(ram));
        memset(wram, 0, sizeof(wram));
    }

    // ----------------------------
    // ROM
    // ----------------------------
    void loadROM(const uint8_t* data, uint32_t size);

    // ----------------------------
    // READ
    // ----------------------------
    uint8_t  read8(uint32_t addr);
    uint16_t read16(uint32_t addr);
    uint32_t read32(uint32_t addr);

    // ----------------------------
    // WRITE
    // ----------------------------
    void write8(uint32_t addr, uint8_t v);
    void write16(uint32_t addr, uint16_t v);
    void write32(uint32_t addr, uint32_t v);
};
