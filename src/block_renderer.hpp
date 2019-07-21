#pragma once

#include <stdexcept>
#include <string>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <unordered_map>

#include <iostream>

#include "glfw.hpp"
#include "glm.hpp"
#include <stb_image.h>

#include "shaders.hpp"
#include "world.hpp"
#include "blocks.hpp"

#define TAU 6.28318531f

struct FaceData {
	float offsetX;
	float offsetY;
	float offsetZ;
	uint8_t side;
	uint32_t texId;
} __attribute__((packed));
// ^^^ It works without the __attribute__, but adding it allows sending less data to the GPU

class RenderedChunk {
public:
	RenderedChunk();
	void init(uint32_t faceVBO, uint32_t faceEBO);
	
	void load(Chunk& chunk);
	void render();
	
private:
	uint32_t VBO, VAO;
	int faceCount;
};

class BlockRenderer {
public:
	void init();
	
	void renderChunk(Chunk& chunk, int32_t x, int32_t z);
	void render(glm::mat4 proj, glm::mat4 view, int32_t camChunkX, int32_t chamChunkZ, const float skyColor[3]);
	
private:
	uint32_t program, faceVBO, faceEBO, textureArray;
	static const int renderDist = 2;
	static constexpr float fogEnd = renderDist * CHUNK_SIZE;
	static constexpr float fogStart = fogEnd * 0.9f;
	
	std::unordered_map<uint64_t, RenderedChunk> renderedChunks;
};
