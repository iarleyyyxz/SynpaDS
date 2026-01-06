#pragma once
#include <cstdint>
#include <cstdio>
#include "../memory/memory.h"

struct CPU {

    // -------------------------------------------------
    // REGISTERS
    // -------------------------------------------------
    uint32_t R[16];     // R0-R15
    uint32_t CPSR;      // Current Program Status Register
    Memory* mem;

    enum FLAGS {
        N = 1 << 31,
        Z = 1 << 30,
        C = 1 << 29,
        V = 1 << 28,
        T = 1 << 5
    };

    CPU(Memory* memory) : mem(memory) {
        reset();
    }

    void reset() {
        for (auto& r : R) r = 0;
        CPSR = 0;
        PC() = 0;
    }

    inline uint32_t& PC() { return R[15]; }

    // -------------------------------------------------
    // FLAGS
    // -------------------------------------------------
    inline void setFlag(uint32_t f, bool v) {
        if (v) CPSR |= f;
        else CPSR &= ~f;
    }

    inline bool getFlag(uint32_t f) const {
        return CPSR & f;
    }

    inline void setNZ(uint32_t v) {
        setFlag(Z, v == 0);
        setFlag(N, v & 0x80000000);
    }

    // -------------------------------------------------
    // FETCH
    // -------------------------------------------------
    uint32_t fetch32() {
        uint32_t v = mem->read32(PC());
        PC() = (PC() + 4) & ~3;
        return v;
    }

    uint16_t fetch16() {
        uint16_t v = mem->read16(PC());
        PC() = (PC() + 2) & ~1;
        return v;
    }

    // -------------------------------------------------
    // STEP
    // -------------------------------------------------
    void step() {
        if (getFlag(T)) {
            decodeThumb(fetch16());
        }
        else {
            decodeARM(fetch32());
        }
    }

    // =================================================
    // =================== THUMB =======================
    // =================================================
    void decodeThumb(uint16_t instr) {
        uint16_t op = instr >> 13;

        switch (op) {
        case 0b000: thumbShift(instr); break;
        case 0b001: thumbImm(instr); break;
        case 0b010: thumbALU(instr); break;
        case 0b101: thumbBranch(instr); break;
        default:
            printf("UNIMPL THUMB %04X\n", instr);
            break;
        }
    }

    void thumbShift(uint16_t instr) {
        uint32_t type = (instr >> 11) & 3;
        uint32_t imm = (instr >> 6) & 0x1F;
        uint32_t Rs = (instr >> 3) & 7;
        uint32_t Rd = instr & 7;

        uint32_t v = R[Rs];
        uint32_t r = 0;

        if (type == 0) r = v << imm;
        else if (type == 1) r = v >> imm;
        else r = ((int32_t)v) >> imm;

        R[Rd] = r;
        setNZ(r);
    }

    void thumbImm(uint16_t instr) {
        uint32_t op = (instr >> 11) & 3;
        uint32_t Rd = (instr >> 8) & 7;
        uint32_t imm = instr & 0xFF;

        uint32_t r;
        if (op == 0) R[Rd] = imm;
        else if (op == 1) { r = R[Rd] - imm; setNZ(r); setFlag(C, R[Rd] >= imm); return; }
        else if (op == 2) R[Rd] += imm;
        else R[Rd] -= imm;

        setNZ(R[Rd]);
    }

    void thumbALU(uint16_t instr) {
        uint32_t op = (instr >> 6) & 0xF;
        uint32_t Rs = (instr >> 3) & 7;
        uint32_t Rd = instr & 7;

        uint32_t a = R[Rd], b = R[Rs], r = 0;

        switch (op) {
        case 0x0: r = a & b; break;
        case 0x1: r = a ^ b; break;
        case 0xC: r = a | b; break;
        case 0xD: r = b; break;
        default:
            printf("UNIMPL THUMB ALU %X\n", op);
            return;
        }

        R[Rd] = r;
        setNZ(r);
    }

    void thumbBranch(uint16_t instr) {
        int32_t off = instr & 0x7FF;
        if (off & 0x400) off |= 0xFFFFF800;
        PC() += off << 1;
    }

    // =================================================
    // =================== ARM =========================
    // =================================================
    bool checkCond(uint32_t cond) {
        switch (cond) {
        case 0x0: return getFlag(Z);
        case 0x1: return !getFlag(Z);
        case 0xE: return true;
        default: return true;
        }
    }

    void decodeARM(uint32_t instr) {

        if (!checkCond(instr >> 28)) return;

        // SWI
        if ((instr & 0x0F000000) == 0x0F000000) {
            execSWI(instr);
            return;
        }

        // BX / BLX
        if ((instr & 0x0FFFFFF0) == 0x012FFF10 ||
            (instr & 0x0FFFFFF0) == 0x012FFF30) {
            execBX(instr);
            return;
        }

        // LDM / STM
        if ((instr & 0x0E000000) == 0x08000000) {
            execLDMSTM(instr);
            return;
        }

        // Branch
        if ((instr & 0x0E000000) == 0x0A000000) {
            execBranch(instr);
            return;
        }

        // Data processing
        if ((instr & 0x0C000000) == 0x00000000 ||
            (instr & 0x0C000000) == 0x02000000) {
            execDP(instr);
            return;
        }

        // Load / Store
        if ((instr & 0x0C000000) == 0x04000000) {
            execLoadStore(instr);
            return;
        }

        printf("UNKNOWN ARM %08X\n", instr);
    }

    // -------------------------------------------------
    // ARM HELPERS
    // -------------------------------------------------
    uint32_t operand2(uint32_t instr) {
        if (instr & (1 << 25)) {
            uint32_t imm = instr & 0xFF;
            uint32_t rot = ((instr >> 8) & 0xF) * 2;
            return (imm >> rot) | (imm << (32 - rot));
        }
        return R[instr & 0xF];
    }

    void execDP(uint32_t instr) {
        uint32_t op = (instr >> 21) & 0xF;
        bool s = instr & (1 << 20);
        uint32_t Rn = (instr >> 16) & 0xF;
        uint32_t Rd = (instr >> 12) & 0xF;
        uint32_t op2 = operand2(instr);

        uint32_t r = 0;

        switch (op) {
        case 0x0: r = R[Rn] & op2; break;
        case 0x1: r = R[Rn] ^ op2; break;
        case 0x2: r = R[Rn] - op2; break;
        case 0x4: r = R[Rn] + op2; break;
        case 0xA: r = R[Rn] - op2; setNZ(r); return;
        case 0xC: r = R[Rn] | op2; break;
        case 0xD: r = op2; break;
        default:
            printf("UNIMPL DP %X\n", op);
            return;
        }

        R[Rd] = r;
        if (s) setNZ(r);
    }

    void execBranch(uint32_t instr) {
        int32_t off = instr & 0x00FFFFFF;
        if (off & 0x00800000) off |= 0xFF000000;
        PC() += off << 2;
    }

    void execBX(uint32_t instr) {
        uint32_t rm = instr & 0xF;
        uint32_t target = R[rm];
        setFlag(T, target & 1);
        PC() = target & ~1;
    }

    void execLDMSTM(uint32_t instr) {
        bool P = instr & (1 << 24);
        bool U = instr & (1 << 23);
        bool W = instr & (1 << 21);
        bool L = instr & (1 << 20);

        uint32_t rn = (instr >> 16) & 0xF;
        uint32_t list = instr & 0xFFFF;

        uint32_t base = R[rn];
        int count = __builtin_popcount(list);

        uint32_t addr = base;
        if (U) addr += P ? 4 : 0;
        else addr -= P ? count * 4 : (count - 1) * 4;

        for (int i = 0; i < 16; i++) {
            if (list & (1 << i)) {
                if (L) R[i] = mem->read32(addr);
                else mem->write32(addr, R[i]);
                addr += 4;
            }
        }

        if (W) R[rn] = U ? base + count * 4 : base - count * 4;
    }

    void execLoadStore(uint32_t instr) {
        bool L = instr & (1 << 20);
        bool B = instr & (1 << 22);
        bool U = instr & (1 << 23);
        bool P = instr & (1 << 24);
        bool W = instr & (1 << 21);

        uint32_t Rn = (instr >> 16) & 0xF;
        uint32_t Rd = (instr >> 12) & 0xF;
        uint32_t off = instr & 0xFFF;

        uint32_t base = R[Rn];
        uint32_t addr = P ? (U ? base + off : base - off) : base;

        if (L) R[Rd] = B ? mem->read8(addr) : mem->read32(addr);
        else B ? mem->write8(addr, R[Rd]) : mem->write32(addr, R[Rd]);

        if (!P) addr = U ? base + off : base - off;
        if (W) R[Rn] = addr;
    }

    void execSWI(uint32_t instr) {
        printf("SWI %06X\n", instr & 0xFFFFFF);
    }
};
