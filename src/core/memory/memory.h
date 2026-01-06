#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "../timers/timers.h"

struct Memory {

    static constexpr uint32_t BIOS_SIZE = 0x4000;      // 16 KB
    static constexpr uint32_t MAIN_RAM_SIZE = 4 * 1024 * 1024; // 4 MB
    static constexpr uint32_t IO_SIZE = 0x1000;

    uint8_t bios[BIOS_SIZE];
    uint8_t mainRAM[MAIN_RAM_SIZE];
    uint8_t io[IO_SIZE];

    // IRQ registers
    uint32_t IME = 0;
    uint32_t IE = 0;
    uint32_t IF = 0;

    Timers timers;

    Memory();

    uint8_t  read8(uint32_t addr);
    uint16_t read16(uint32_t addr);
    uint32_t read32(uint32_t addr);

    void write8(uint32_t addr, uint8_t v);
    void write16(uint32_t addr, uint16_t v);
    void write32(uint32_t addr, uint32_t v);
};
