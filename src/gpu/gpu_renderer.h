#pragma once

#include <cstdint>

struct GPURenderer {

	virtual void renderFrame(const uint8_t* vram) = 0;
	virtual void clear() = 0;
};