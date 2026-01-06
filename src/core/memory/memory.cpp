#include "memory.h"

Memory::Memory() {
    memset(bios, 0, sizeof(bios));
    memset(mainRAM, 0, sizeof(mainRAM));
    memset(io, 0, sizeof(io));
}

// ---------------- READ ----------------

uint8_t Memory::read8(uint32_t addr) {

    // BIOS
    if (addr < BIOS_SIZE)
        return bios[addr];

    // MAIN RAM (0x02000000)
    if (addr >= 0x02000000 && addr < 0x02000000 + MAIN_RAM_SIZE)
        return mainRAM[addr - 0x02000000];

    // I/O
    if (addr >= 0x04000000 && addr < 0x04001000) {
        uint32_t off = addr - 0x04000000;

        if (off == 0x208) return IME;
        if (off == 0x210) return IE;
        if (off == 0x214) return IF;

        return io[off];
    }

    return 0;
}

uint16_t Memory::read16(uint32_t addr) {
    // Timers
    if (addr >= 0x04000100 && addr <= 0x0400010F) {
        int id = (addr - 0x04000100) / 4;
        bool high = addr & 2;
        return high ? timers.readCNT_H(id) : timers.readCNT_L(id);
    }
    return read8(addr) | (read8(addr + 1) << 8);
}

uint32_t Memory::read32(uint32_t addr) {
    return read8(addr) |
        (read8(addr + 1) << 8) |
        (read8(addr + 2) << 16) |
        (read8(addr + 3) << 24);
}

// ---------------- WRITE ----------------

void Memory::write8(uint32_t addr, uint8_t v) {

    // MAIN RAM
    if (addr >= 0x02000000 && addr < 0x02000000 + MAIN_RAM_SIZE) {
        mainRAM[addr - 0x02000000] = v;
        return;
    }

    // I/O
    if (addr >= 0x04000000 && addr < 0x04001000) {
        uint32_t off = addr - 0x04000000;

        if (off == 0x208) IME = v;
        else if (off == 0x210) IE = v;
        else if (off == 0x214) IF &= ~v; // write-1-to-clear
        else io[off] = v;
    }
}

void Memory::write16(uint32_t addr, uint16_t v) {
    if (addr >= 0x04000100 && addr <= 0x0400010F) {
        int id = (addr - 0x04000100) / 4;
        bool high = addr & 2;
        if (high) timers.writeCNT_H(id, v);
        else timers.writeCNT_L(id, v);
        return;
    }

    write8(addr, v & 0xFF);
    write8(addr + 1, v >> 8);
}

void Memory::write32(uint32_t addr, uint32_t v) {

    // I/O registers
    if (addr == 0x04000208) { IME = v; return; }
    if (addr == 0x04000210) { IE = v; return; }
    if (addr == 0x04000214) { IF &= ~v; return; }

    write8(addr, v & 0xFF);
    write8(addr + 1, (v >> 8) & 0xFF);
    write8(addr + 2, (v >> 16) & 0xFF);
    write8(addr + 3, (v >> 24) & 0xFF);
}
