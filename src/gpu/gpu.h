#pragma once
#include <cstdint>
#include <vector>
#include "../gpu/gpu_renderer.h"

// Tamanho da tela do Nintendo DS
constexpr int DS_WIDTH = 256;
constexpr int DS_HEIGHT = 192;
constexpr int VRAM_SIZE = DS_WIDTH * DS_HEIGHT * 4; // RGBA 8 bits por canal

struct GPU {
    std::vector<uint8_t> vram;   // VRAM do DS
    GPURenderer* renderer;          // Ponteiro para renderizador

    GPU(GPURenderer* r) : vram(VRAM_SIZE, 0), renderer(r) {}

    // Escreve pixel na VRAM
    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        if (x < 0 || x >= DS_WIDTH || y < 0 || y >= DS_HEIGHT) return;
        int idx = (y * DS_WIDTH + x) * 4;
        vram[idx + 0] = r;
        vram[idx + 1] = g;
        vram[idx + 2] = b;
        vram[idx + 3] = a;
    }

    // Atualiza o frame
    void renderFrame() {
        if (renderer) {
            renderer->renderFrame(vram.data());
        }
    }

    void clear() {
        if (renderer) {
            renderer->clear();
        }
        std::fill(vram.begin(), vram.end(), 0);
    }
};
