#pragma once

#include "blocks.hpp"

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 64
#define CHUNK_BLOCKS (CHUNK_SIZE*CHUNK_SIZE*CHUNK_HEIGHT)
#define MAX_CHUNK_FACES (CHUNK_BLOCKS*3)

#define BLOCK_IDX(x, y, z) (x + CHUNK_SIZE*z + CHUNK_SIZE*CHUNK_SIZE*y)

class Chunk {
public:
	Chunk();
	
	Block* getBlock(uint8_t x, uint8_t y, uint8_t z);
	bool isOpaqueCube(uint8_t x, uint8_t y, uint8_t z);
	
	void setBlock(uint8_t x, uint8_t y, uint8_t z, Block& block);
	
private:
	BlockId blocks[CHUNK_BLOCKS];
};