#pragma once

#include <unordered_map>
#include <utility>
#include <tuple>

#include "glm.hpp"

#include "chunk.hpp"
#include "worldgen.hpp"
#include "blocks.hpp"

uint64_t getChunkId(int32_t x, int32_t z);

class World {
public:
	World();
	
	static std::pair<int32_t, int32_t> getChunkAt(int32_t x, int32_t z);
	
	bool isChunkLoaded(int32_t x, int32_t z);
	Chunk& getChunk(int32_t x, int32_t z);
	Chunk& genChunk(int32_t x, int32_t z);
	
	Block* getBlock(int32_t x, int32_t y, int32_t z);
	void setBlock(int32_t x, int32_t y, int32_t z, Block& block);
	void removeBlock(int32_t x, int32_t y, int32_t z);
	
	glm::vec3 playerPos, playerOrient;
	
private:
	WorldGenerator gen;
	std::unordered_map<uint64_t, Chunk> loadedChunks;
};
