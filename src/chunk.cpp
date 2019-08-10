#include "chunk.hpp"

#include <stdexcept>

Chunk::Chunk() : blocks(), opaqueCubeCache() { }

bool Chunk::hasBlock(uint8_t x, uint8_t y, uint8_t z) {
	if(INVALID_BLOCK_POS(x, y, z)) return false;
	return blocks[BLOCK_IDX(x, y, z)] != 0;
}

Block* Chunk::getBlock(uint8_t x, uint8_t y, uint8_t z) {
	if(INVALID_BLOCK_POS(x, y, z)) return nullptr;
	BlockId id = blocks[BLOCK_IDX(x, y, z)];
	if(id != 0) {
		return &Block::fromId(id);
	} else {
		return nullptr;
	}
}

void Chunk::setBlock(uint8_t x, uint8_t y, uint8_t z, Block& block) {
	if(INVALID_BLOCK_POS(x, y, z)) throw std::logic_error("Invalid block position in chunk");
	blocks[BLOCK_IDX(x, y, z)] = block.id();
	opaqueCubeCache[BLOCK_IDX(x, y, z)] = block.isOpaqueCube();
}

void Chunk::removeBlock(uint8_t x, uint8_t y, uint8_t z) {
	if(INVALID_BLOCK_POS(x, y, z)) throw std::logic_error("Invalid block position in chunk");
	blocks[BLOCK_IDX(x, y, z)] = 0;
	opaqueCubeCache[BLOCK_IDX(x, y, z)] = false;
}

bool Chunk::isOpaqueCube(uint8_t x, uint8_t y, uint8_t z) {
	return opaqueCubeCache[BLOCK_IDX(x, y, z)];
}

void Chunk::setBlockId(uint8_t x, uint8_t y, uint8_t z, BlockId id, bool isOpaqueCube) {
	blocks[BLOCK_IDX(x, y, z)] = id;
	opaqueCubeCache[BLOCK_IDX(x, y, z)] = isOpaqueCube;
}
