#pragma once
#include <cstdint>
#include <cstring>

constexpr uint32_t MEM_SIZE = 1024 * 1024; // 1 MB

struct Memory {
	uint8_t data[MEM_SIZE];

	Memory() {
		std::memset(data, 0, sizeof(data));
	}

    uint32_t read32(uint32_t addr) const {
        if (addr + 3 >= MEM_SIZE) {
            printf("ERRO: endereço inválido na leitura 0x%08X\n", addr);
            return 0;
        }
        return data[addr] | (data[addr + 1] << 8) | (data[addr + 2] << 16) | (data[addr + 3] << 24);
    }


    void write32(uint32_t addr, uint32_t value) {
        if (addr + 3 >= MEM_SIZE) {
            printf("ERRO: write32 endereço inválido 0x%08X\n", addr);
            return;
        }
        data[addr] = value & 0xFF;
        data[addr + 1] = (value >> 8) & 0xFF;
        data[addr + 2] = (value >> 16) & 0xFF;
        data[addr + 3] = (value >> 24) & 0xFF;
    }


};


