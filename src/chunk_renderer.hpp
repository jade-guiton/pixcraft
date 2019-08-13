#pragma once

#include <unordered_map>
#include <unordered_set>

#include <stb_image.h>

#include "world/world.hpp"
#include "face_renderer.hpp"

class RenderedChunk {
public:
	void init(World& world, FaceRenderer& faceRenderer, int32_t chunkX, int32_t chunkZ);
	bool isInitialized();
	
	void prerender();
	void updateBuffers();
	
	void updateBlock(int8_t relX, int8_t y, int8_t relZ);
	void updatePlaneX(int8_t relX);
	void updatePlaneZ(int8_t relZ);
	
	void render(FaceRenderer& faceRenderer);
	void renderTranslucent(FaceRenderer& faceRenderer);
	
private:
	World* world;
	FaceBuffer buffer;
	FaceBuffer translucentBuffer;
	int32_t chunkX, chunkZ;
	
	void prerenderBlock(Chunk& chunk, int8_t relX, int8_t y, int8_t relZ);
};

class ChunkRenderer {
public:
	ChunkRenderer(World& world, FaceRenderer& renderer, int renderDist);
	
	bool isChunkRendered(int32_t chunkX, int32_t chunkZ);
	size_t renderedChunkCount();
	
	void prerenderChunk(int32_t chunkX, int32_t chunkZ);
	void updateBlocks();
	
	void render(int32_t camChunkX, int32_t chamChunkZ);
	void renderTranslucent(int32_t camChunkX, int32_t chamChunkZ);
	
private:
	World& world;
	FaceRenderer& faceRenderer;
	const int renderDist;
	
	std::unordered_map<uint64_t, RenderedChunk> renderedChunks;
	
	void updateBlock(std::unordered_set<uint64_t>& updated, int32_t x, int32_t y, int32_t z);
};
