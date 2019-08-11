#pragma once

#include <unordered_map>

#include <stb_image.h>

#include "world.hpp"
#include "face_renderer.hpp"

class RenderedChunk {
public:
	void init(FaceRenderer& faceRenderer, World& world, int32_t chunkX, int32_t chunkZ);
	bool isInitialized();
	
	void prerender();
	void updateBlock(int8_t relX, int8_t y, int8_t relZ);
	void updatePlaneX(int8_t relX);
	void updatePlaneZ(int8_t relZ);
	
	void render(FaceRenderer& faceRenderer);
	void renderTranslucent(FaceRenderer& faceRenderer);
	
private:
	FaceBuffer buffer;
	FaceBuffer translucentBuffer;
	World* world;
	int32_t chunkX, chunkZ;
	
	void prerenderBlock(Chunk& chunk, int8_t relX, int8_t y, int8_t relZ);
};

class ChunkRenderer {
public:
	ChunkRenderer(FaceRenderer& renderer, int renderDist);
	
	bool isChunkRendered(int32_t chunkX, int32_t chunkZ);
	size_t renderedChunkCount();
	
	void prerenderChunk(World& world, int32_t chunkX, int32_t chunkZ);
	void updateBlocks(World& world);
	
	void render(int32_t camChunkX, int32_t chamChunkZ);
	
private:
	FaceRenderer& faceRenderer;
	const int renderDist;
	
	std::unordered_map<uint64_t, RenderedChunk> renderedChunks;
	
	void updateSingleBlock(World& world, int32_t x, int32_t y, int32_t z);
	void updateBlock(World& world, int32_t x, int32_t y, int32_t z);
};
