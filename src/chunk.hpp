#pragma once

#include "blocks.hpp"

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 64
#define CHUNK_BLOCKS (CHUNK_SIZE*CHUNK_SIZE*CHUNK_HEIGHT)
#define MAX_CHUNK_FACES (CHUNK_BLOCKS*3)

#define BLOCK_IDX(x, y, z) (x + CHUNK_SIZE*z + CHUNK_SIZE*CHUNK_SIZE*y)
#define INVALID_BLOCK_POS(x, y, z) (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE)

class Chunk {
public:
	Chunk();
	
	bool hasBlock(uint8_t x, uint8_t y, uint8_t z);
	Block* getBlock(uint8_t x, uint8_t y, uint8_t z);
	void setBlock(uint8_t x, uint8_t y, uint8_t z, Block& block);
	void removeBlock(uint8_t x, uint8_t y, uint8_t z);
	
	// Warning: the following functions do not check that the given position is valid.
	bool isOpaqueCube(uint8_t x, uint8_t y, uint8_t z);
	void setBlockId(uint8_t x, uint8_t y, uint8_t z, BlockId id, bool isOpaqueCube);
	
private:
	BlockId blocks[CHUNK_BLOCKS];
	bool opaqueCubeCache[CHUNK_BLOCKS];
};
