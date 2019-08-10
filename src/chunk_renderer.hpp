#pragma once

#include <unordered_map>

#include <stb_image.h>

#include "chunk.hpp"
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
