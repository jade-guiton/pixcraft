#pragma once

#include <stdexcept>
#include <string>
#include <cstddef>
#include <vector>
#include <unordered_map>

#include <iostream>

#include "glfw.hpp"
#include "glm.hpp"
#include <stb_image.h>

#include "shaders.hpp"
#include "world.hpp"
#include "blocks.hpp"
#include "chunk.hpp"
#include "util.hpp"
#include "face_renderer.hpp"

class RenderedChunk {
public:
	void init(FaceRenderer& faceRenderer);
	bool isInitialized();
	
	void load(Chunk& chunk);
	void render(FaceRenderer& faceRenderer, int32_t x, int32_t z);
	
private:
	FaceBuffer buffer;
};

class ChunkRenderer {
public:
	ChunkRenderer(FaceRenderer& renderer, int renderDist);
	
	void prerenderChunk(Chunk& chunk, int32_t x, int32_t z);
	void render(int32_t camChunkX, int32_t chamChunkZ);
	
private:
	FaceRenderer& faceRenderer;
	const int renderDist;
	
	std::unordered_map<uint64_t, RenderedChunk> renderedChunks;
};
