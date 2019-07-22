#include "chunk.hpp"

Chunk::Chunk() : blocks(), opaqueCubeCache() { }

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

bool Chunk::isOpaqueCube(uint8_t x, uint8_t y, uint8_t z) {
	return opaqueCubeCache[BLOCK_IDX(x, y, z)];
}

void Chunk::setBlockId(uint8_t x, uint8_t y, uint8_t z, BlockId id, bool isOpaqueCube) {
	blocks[BLOCK_IDX(x, y, z)] = id;
	opaqueCubeCache[BLOCK_IDX(x, y, z)] = isOpaqueCube;
}
