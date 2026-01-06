#include "memory.h"

uint8_t Memory::read8(uint32_t addr) const {
    if (addr >= MEM_SIZE) {
        printf("ERRO: read8 invalid adress 0x%08X\n", addr);
        return 0;
    }
    return data[addr];
}

uint16_t Memory::read16(uint32_t addr) const {
    if (addr + 1 >= MEM_SIZE) {
        printf("ERRO: read16 invalid adress 0x%08X\n", addr);
        return 0;
    }
    return data[addr] |
        (data[addr + 1] << 8);
}

uint32_t Memory::read32(uint32_t addr) const {
    if (addr + 3 >= MEM_SIZE) {
        printf("ERRO: read32 invalid adress 0x%08X\n", addr);
        return 0;
    }
    return data[addr] |
        (data[addr + 1] << 8) |
        (data[addr + 2] << 16) |
        (data[addr + 3] << 24);
}

void Memory::write8(uint32_t addr, uint8_t value) {
    if (addr >= MEM_SIZE) {
        printf("ERRO: write8 invalid adress 0x%08X\n", addr);
        return;
    }
    data[addr] = value;
}

void Memory::write16(uint32_t addr, uint16_t value) {
    if (addr + 1 >= MEM_SIZE) {
        printf("ERRO: write16 invalid adress 0x%08X\n", addr);
        return;
    }
    data[addr] = value & 0xFF;
    data[addr + 1] = (value >> 8) & 0xFF;
}

void Memory::write32(uint32_t addr, uint32_t value) {
    if (addr + 3 >= MEM_SIZE) {
        printf("ERRO: write32 invalid adress 0x%08X\n", addr);
        return;
    }
    data[addr] = value & 0xFF;
    data[addr + 1] = (value >> 8) & 0xFF;
    data[addr + 2] = (value >> 16) & 0xFF;
    data[addr + 3] = (value >> 24) & 0xFF;
}
