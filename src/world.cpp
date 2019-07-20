#include "world.hpp"

Chunk::Chunk() : blocks() { }

Block* Chunk::getBlock(uint8_t x, uint8_t y, uint8_t z) {
	if(x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
		return nullptr;
	BlockId id = blocks[BLOCK_IDX(x, y, z)];
	if(id != 0) {
		return Block::fromId(id);
	} else {
		return nullptr;
	}
}

bool Chunk::isOpaqueCube(uint8_t x, uint8_t y, uint8_t z) {
	Block* block = getBlock(x, y, z);
	return block != nullptr && block->isOpaqueCube();
}

void Chunk::setBlock(uint8_t x, uint8_t y, uint8_t z, Block& block) {
	blocks[BLOCK_IDX(x, y, z)] = block.getId();
}
