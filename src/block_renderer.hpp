#pragma once

#include <stdexcept>
#include <string>
#include <cstddef>

#include "glfw.hpp"
#include "glm.hpp"
#include <stb_image.h>

#include "shaders.hpp"

#define TAU 6.28318531f

struct FaceData {
	float offsetX;
	float offsetY;
	float offsetZ;
	uint8_t side;
	uint32_t texId;
} __attribute__((packed));
// ^^^ It works without the __attribute__, but adding it allows sending less data to the GPU

class BlockRenderer {
public:
	void init();
	
	void render(glm::mat4 proj, glm::mat4 view);
	
private:
	unsigned int program, VAO, textureArray;
};
