#include "memory.h"

// -------------------------------------------------
// ROM
// -------------------------------------------------
void Memory::loadROM(const uint8_t* data, uint32_t size) {
    romSize = size > ROM_MAX ? ROM_MAX : size;
    memcpy(rom, data, romSize);
}

// -------------------------------------------------
// READ
// -------------------------------------------------
uint8_t Memory::read8(uint32_t addr) {

    // BIOS
    if (addr < BIOS_SIZE)
        return bios[addr];

    // Main RAM
    if (addr >= 0x02000000 && addr < 0x02000000 + RAM_SIZE)
        return ram[addr - 0x02000000];

    // WRAM
    if (addr >= 0x03000000 && addr < 0x03000000 + WRAM_SIZE)
        return wram[addr - 0x03000000];

    // ROM
    if (addr >= 0x08000000 && addr < 0x08000000 + romSize)
        return rom[addr - 0x08000000];

    printf("read8 inválido 0x%08X\n", addr);
    return 0;
}

uint16_t Memory::read16(uint32_t addr) {
    return read8(addr) | (read8(addr + 1) << 8);
}

uint32_t Memory::read32(uint32_t addr) {
    return read8(addr) |
        (read8(addr + 1) << 8) |
        (read8(addr + 2) << 16) |
        (read8(addr + 3) << 24);
}

// -------------------------------------------------
// WRITE
// -------------------------------------------------
void Memory::write8(uint32_t addr, uint8_t v) {

    // BIOS é read-only
    if (addr < BIOS_SIZE)
        return;

    if (addr >= 0x02000000 && addr < 0x02000000 + RAM_SIZE) {
        ram[addr - 0x02000000] = v;
        return;
    }

    if (addr >= 0x03000000 && addr < 0x03000000 + WRAM_SIZE) {
        wram[addr - 0x03000000] = v;
        return;
    }

    printf("write8 inválido 0x%08X\n", addr);
}

void Memory::write16(uint32_t addr, uint16_t v) {
    write8(addr, v & 0xFF);
    write8(addr + 1, (v >> 8) & 0xFF);
}

void Memory::write32(uint32_t addr, uint32_t v) {
    write8(addr, v & 0xFF);
    write8(addr + 1, (v >> 8) & 0xFF);
    write8(addr + 2, (v >> 16) & 0xFF);
    write8(addr + 3, (v >> 24) & 0xFF);
}
