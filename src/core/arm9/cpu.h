#pragma once
#include <cstdint>
#include <cstdio>
#include "../memory/memory.h"

struct CPU {
    uint32_t R[16];   // R0-R15
    uint32_t CPSR;    // Flags

    Memory* mem;

    enum FLAGS {
        N = 1 << 31,
        Z = 1 << 30,
        C = 1 << 29,
        V = 1 << 28,
        T = 1 << 5     // THUMB
    };

    CPU(Memory* memory) : mem(memory) {
        reset();
    }

    void reset() {
        for (auto& r : R) r = 0;
        CPSR = 0;
        PC() = 0x00000000;
    }

    uint32_t& PC() { return R[15]; }

    // -------------------------------------------------
    // FLAGS
    // -------------------------------------------------
    inline void setFlag(uint32_t flag, bool v) {
        if (v) CPSR |= flag;
        else CPSR &= ~flag;
    }

    inline bool getFlag(uint32_t flag) const {
        return CPSR & flag;
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
        PC() += 4;
        return v;
    }

    uint16_t fetch16() {
        uint16_t v = mem->read16(PC());
        PC() += 2;
        return v;
    }

    // -------------------------------------------------
    // STEP
    // -------------------------------------------------
    void step() {
        if (getFlag(T)) {
            // THUMB (placeholder)
            uint16_t instr = fetch16();
            printf("THUMB instr %04X\n", instr);
        }
        else {
            uint32_t instr = fetch32();
            decodeExecute(instr);
        }
    }

    // -------------------------------------------------
    // CONDITIONS
    // -------------------------------------------------
    bool checkCondition(uint32_t cond) {
        switch (cond) {
        case 0x0: return getFlag(Z);                       // EQ
        case 0x1: return !getFlag(Z);                      // NE
        case 0xA: return getFlag(N);                       // MI
        case 0xB: return !getFlag(N);                      // PL
        case 0xC: return getFlag(V);                       // VS
        case 0xD: return !getFlag(V);                      // VC
        case 0xE: return true;                             // AL
        default: return true;
        }
    }

    // -------------------------------------------------
    // DECODE
    // -------------------------------------------------
    void decodeExecute(uint32_t instr) {
        if (!checkCondition(instr >> 28)) return;

        if ((instr & 0x0E000000) == 0x0A000000) {
            execBranch(instr);
        }
        else if ((instr & 0x0C000000) == 0x00000000 ||
            (instr & 0x0C000000) == 0x02000000) {
            execDataProcessing(instr);
        }
        else if ((instr & 0x0C000000) == 0x04000000) {
            execLoadStore(instr);
        }
        else {
            printf("UNKNOWN ARM INSTR %08X\n", instr);
        }
    }

    // -------------------------------------------------
    // DATA PROCESSING
    // -------------------------------------------------
    uint32_t getOperand2(uint32_t instr) {
        if (instr & (1 << 25)) {
            uint32_t imm = instr & 0xFF;
            uint32_t rot = ((instr >> 8) & 0xF) * 2;
            return (imm >> rot) | (imm << (32 - rot));
        }
        return R[instr & 0xF];
    }

    void execADD(uint32_t Rd, uint32_t Rn, uint32_t op2, bool s) {
        uint64_t res = (uint64_t)R[Rn] + op2;
        R[Rd] = (uint32_t)res;
        if (s) {
            setNZ(R[Rd]);
            setFlag(C, res >> 32);
            setFlag(V, (~(R[Rn] ^ op2) & (R[Rn] ^ R[Rd])) & 0x80000000);
        }
    }

    void execSUB(uint32_t Rd, uint32_t Rn, uint32_t op2, bool s) {
        uint64_t res = (uint64_t)R[Rn] - op2;
        uint32_t r = (uint32_t)res;
        if (Rd != 0xFFFFFFFF) R[Rd] = r;
        if (s) {
            setNZ(r);
            setFlag(C, R[Rn] >= op2);
            setFlag(V, ((R[Rn] ^ op2) & (R[Rn] ^ r)) & 0x80000000);
        }
    }

    void execDataProcessing(uint32_t instr) {
        uint32_t opcode = (instr >> 21) & 0xF;
        bool s = instr & (1 << 20);
        uint32_t Rn = (instr >> 16) & 0xF;
        uint32_t Rd = (instr >> 12) & 0xF;
        uint32_t op2 = getOperand2(instr);

        switch (opcode) {
        case 0x0: R[Rd] = R[Rn] & op2; if (s) setNZ(R[Rd]); break; // AND
        case 0x1: R[Rd] = R[Rn] ^ op2; if (s) setNZ(R[Rd]); break; // EOR
        case 0x2: execSUB(Rd, Rn, op2, s); break;                // SUB
        case 0x4: execADD(Rd, Rn, op2, s); break;                // ADD
        case 0xA: execSUB(0xFFFFFFFF, Rn, op2, true); break;     // CMP
        case 0xC: R[Rd] = R[Rn] | op2; if (s) setNZ(R[Rd]); break;// ORR
        case 0xD: R[Rd] = op2; if (s) setNZ(R[Rd]); break;       // MOV
        default:
            printf("UNIMPL DP %X\n", opcode);
        }
    }

    // -------------------------------------------------
    // BRANCH
    // -------------------------------------------------
    void execBranch(uint32_t instr) {
        int32_t offset = instr & 0x00FFFFFF;
        if (offset & 0x00800000) offset |= 0xFF000000;
        offset <<= 2;
        PC() += offset;
    }

    // -------------------------------------------------
    // LOAD / STORE
    // -------------------------------------------------
    void execLoadStore(uint32_t instr) {
        bool load = instr & (1 << 20);
        bool byte = instr & (1 << 22);
        bool up = instr & (1 << 23);

        uint32_t Rn = (instr >> 16) & 0xF;
        uint32_t Rd = (instr >> 12) & 0xF;
        uint32_t offset = instr & 0xFFF;

        uint32_t addr = R[Rn];
        addr += up ? offset : -offset;

        if (load) {
            R[Rd] = byte ? mem->read8(addr) : mem->read32(addr);
        }
        else {
            if (byte) mem->write8(addr, R[Rd]);
            else mem->write32(addr, R[Rd]);
        }
    }
};
