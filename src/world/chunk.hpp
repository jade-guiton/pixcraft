#pragma once

#include <vector>
#include <unordered_set>
#include <tuple>

#include "world_module.hpp"

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 64
#define CHUNK_BLOCKS (CHUNK_SIZE*CHUNK_SIZE*CHUNK_HEIGHT)
#define MAX_CHUNK_FACES (CHUNK_BLOCKS*3)

#define INVALID_BLOCK_POS(x, y, z) (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE)

class Chunk {
public:
	Chunk();
	void init(World* world);
	
	bool hasBlock(uint8_t x, uint8_t y, uint8_t z);
	Block* getBlock(uint8_t x, uint8_t y, uint8_t z);
	void setBlock(uint8_t x, uint8_t y, uint8_t z, Block& block);
	void removeBlock(uint8_t x, uint8_t y, uint8_t z);
	
	void requestUpdate(uint8_t x, uint8_t y, uint8_t z);
	void updateBlocks(int32_t chunkX, int32_t chunkZ);
	
	// Fast functions; they do not check for invalid positions, and do not update blocks.
	bool isOpaqueCube(uint8_t x, uint8_t y, uint8_t z);
	void setBlockId(uint8_t x, uint8_t y, uint8_t z, BlockId id, bool isOpaqueCube);
	
private:
	World* world;
	BlockId blocks[CHUNK_BLOCKS];
	bool opaqueCubeCache[CHUNK_BLOCKS];
	std::unordered_set<uint32_t> updateRequests;
};
