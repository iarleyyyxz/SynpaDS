#pragma once
#include <cstdint>
#include <cstdio>

#include "memory.h"
#include "../memory/memory.h"


struct CPU {
    uint32_t R[16];   // R0–R15
    uint32_t CPSR;   // flags

    Memory* mem;

    enum FLAGS {
        N = 1 << 31,
        Z = 1 << 30,
        C = 1 << 29,
        V = 1 << 28
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

    // ----------------------------
    // FLAGS
    // ----------------------------
    void setNZ(uint32_t value) {
        if (value == 0) CPSR |= Z;
        else CPSR &= ~Z;

        if (value & 0x80000000) CPSR |= N;
        else CPSR &= ~N;
    }

    // ----------------------------
    // FETCH
    // ----------------------------
    uint32_t fetch() {
        return mem->read32(PC());
    }

    // ----------------------------
    // STEP
    // ----------------------------
    void step() {
        uint32_t pc = PC();
        uint32_t instr = fetch();

        PC() += 4; // pipeline fake

        decodeExecute(instr);
    }

    // ----------------------------
    // DECODE
    // ----------------------------
    void decodeExecute(uint32_t instr) {
        uint32_t cond = instr >> 28;
        if (!checkCondition(cond)) return;

        // Branch
        if ((instr & 0x0E000000) == 0x0A000000) {
            executeBranch(instr);
            return;
        }

        // Data Processing
        if ((instr & 0x0C000000) == 0x00000000 ||
            (instr & 0x0C000000) == 0x02000000) {
            executeDataProcessing(instr);
            return;
        }

        if ((instr & 0x0C000000) == 0x04000000) { // LDR/STR
            executeLoadStore(instr);
            return;
        }

        printf("UNKNOWN INSTR: %08X\n", instr);
    }

    // ----------------------------
    // CONDITION CODES
    // ----------------------------
    bool checkCondition(uint32_t cond) {
        switch (cond) {
        case 0x0: return CPSR & Z;        // EQ
        case 0x1: return !(CPSR & Z);     // NE
        case 0xE: return true;            // AL
        default:  return true;            // simplificado
        }
    }

    // ----------------------------
    // DATA PROCESSING
    // ----------------------------
    void executeDataProcessing(uint32_t instr) {
        bool immediate = instr & (1 << 25);
        uint32_t opcode = (instr >> 21) & 0xF;
        bool setFlags = instr & (1 << 20);

        uint32_t Rn = (instr >> 16) & 0xF;
        uint32_t Rd = (instr >> 12) & 0xF;

        uint32_t operand2 = 0;

        if (immediate) {
            uint32_t imm = instr & 0xFF;
            uint32_t rot = ((instr >> 8) & 0xF) * 2;
            operand2 = (imm >> rot) | (imm << (32 - rot));
        }
        else {
            uint32_t Rm = instr & 0xF;
            operand2 = R[Rm];
        }

        uint32_t result = 0;

        switch (opcode) {
        case 0x0: // AND
            result = R[Rn] & operand2;
            R[Rd] = result;
            if (setFlags) setNZ(result);
			break;
        case 0x1: // EOR
            result = R[Rn] ^ operand2;   // XOR bit a bit
            R[Rd] = result;
            if (setFlags) setNZ(result);
            break;
        case 0x3: // RSB
            result = operand2 - R[Rn];
			R[Rd] = result;
            if (setFlags) setNZ(result);
			break;
        case 0xC: // ORR
            result = R[Rn] | operand2;   // OR bit a bit
            R[Rd] = result;
            if (setFlags) setNZ(result);
            break;
        case 0xD: // MOV
            result = operand2;
            R[Rd] = result;
            if (setFlags) setNZ(result);
            break;

        case 0x4: // ADD
            result = R[Rn] + operand2;
            R[Rd] = result;
            if (setFlags) setNZ(result);
            break;

        case 0x2: // SUB
            result = R[Rn] - operand2;
            R[Rd] = result;
            if (setFlags) setNZ(result);
            break;

        case 0xA: // CMP
            result = R[Rn] - operand2;
            setNZ(result);
            break;


        default:
            printf("UNIMPLEMENTED DP OPCODE: %X\n", opcode);
            break;
        }
    }

    // ----------------------------
    // BRANCH
    // ----------------------------
    void executeBranch(uint32_t instr) {
        int32_t offset = instr & 0x00FFFFFF;
        if (offset & 0x00800000)
            offset |= 0xFF000000; // sign extend

        offset <<= 2;
        PC() += offset;
    }

    void executeLoadStore(uint32_t instr) {
        bool immediate = !(instr & (1 << 25));
        bool up = instr & (1 << 23);
        bool load = instr & (1 << 20);

        uint32_t Rn = (instr >> 16) & 0xF;
        uint32_t Rd = (instr >> 12) & 0xF;
        uint32_t offset = instr & 0xFFF;

        uint32_t addr = R[Rn];
        if (!up) addr -= offset;
        else addr += offset;

        printf("[LDR/STR] instr=0x%08X Rn=R%d(0x%08X) Rd=R%d offset=%d up=%d addr=0x%08X load=%d\n",
            instr, Rn, R[Rn], Rd, offset, up ? 1 : 0, addr, load ? 1 : 0);

        if (addr + 3 >= MEM_SIZE) {
            printf("ERRO: acesso inválido em addr=0x%08X\n", addr);
            return;
        }

        if (load) {
            R[Rd] = mem->read32(addr);
        }
        else {
            mem->write32(addr, R[Rd]);
        }
    }


};
