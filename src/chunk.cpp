#include "chunk.hpp"

#include <stdexcept>

#include "blocks.hpp"
#include "world.hpp"

inline uint32_t blockIdx(uint8_t x, uint8_t y, uint8_t z) {
	return x + CHUNK_SIZE*z + CHUNK_SIZE*CHUNK_SIZE*y;
}

inline uint8_t xFromIdx(uint32_t idx) { return idx % CHUNK_SIZE; }
inline uint8_t yFromIdx(uint32_t idx) { return idx / CHUNK_SIZE / CHUNK_SIZE; }
inline uint8_t zFromIdx(uint32_t idx) { return (idx / CHUNK_SIZE) % CHUNK_SIZE; }

Chunk::Chunk() : world(nullptr), blocks(), opaqueCubeCache() { }

void Chunk::init(World* world2) { world = world2; }

bool Chunk::hasBlock(uint8_t x, uint8_t y, uint8_t z) {
	if(INVALID_BLOCK_POS(x, y, z)) return false;
	return blocks[blockIdx(x, y, z)] != 0;
}

Block* Chunk::getBlock(uint8_t x, uint8_t y, uint8_t z) {
	if(INVALID_BLOCK_POS(x, y, z)) return nullptr;
	BlockId id = blocks[blockIdx(x, y, z)];
	if(id != 0) {
		return &Block::fromId(id);
	} else {
		return nullptr;
	}
}

void Chunk::setBlock(uint8_t x, uint8_t y, uint8_t z, Block& block) {
	if(INVALID_BLOCK_POS(x, y, z)) throw std::logic_error("Invalid block position in chunk");
	blocks[blockIdx(x, y, z)] = block.id();
	opaqueCubeCache[blockIdx(x, y, z)] = block.rendering() == BlockRendering::opaqueCube;
}

void Chunk::removeBlock(uint8_t x, uint8_t y, uint8_t z) {
	if(INVALID_BLOCK_POS(x, y, z)) throw std::logic_error("Invalid block position in chunk");
	blocks[blockIdx(x, y, z)] = 0;
	opaqueCubeCache[blockIdx(x, y, z)] = false;
}

void Chunk::markDirty(uint8_t x, uint8_t y, uint8_t z) {
	dirtyBlocks.insert(blockIdx(x, y, z));
}

void Chunk::requestUpdate(uint8_t x, uint8_t y, uint8_t z) {
	updateRequests.insert(blockIdx(x, y, z));
}

bool Chunk::updateBlocks(int32_t chunkX, int32_t chunkZ) {
	std::unordered_set<uint32_t> updates;
	updateRequests.swap(updates);
	for(uint32_t blockIdx : updates) {
		BlockId id = blocks[blockIdx];
		if(id != 0) {
			int32_t x = CHUNK_SIZE*chunkX + xFromIdx(blockIdx);
			uint8_t y = yFromIdx(blockIdx);
			int32_t z = CHUNK_SIZE*chunkZ + zFromIdx(blockIdx);
			bool res = Block::fromId(id).update(*world, x, y, z);
			if(res) {
				world->requestUpdatesAround(x, y, z);
				dirtyBlocks.insert(blockIdx);
			}
		}
	}
	return dirtyBlocks.size() > 0;
}

std::vector<std::tuple<uint8_t, uint8_t, uint8_t>> Chunk::retrieveDirtyBlocks() {
	std::vector<std::tuple<uint8_t, uint8_t, uint8_t>> res;
	res.reserve(dirtyBlocks.size());
	for(uint32_t blockIdx : dirtyBlocks) {
		uint8_t relX = xFromIdx(blockIdx);
		uint8_t relY = yFromIdx(blockIdx);
		uint8_t relZ = zFromIdx(blockIdx);
		res.push_back(std::tuple<uint8_t, uint8_t, uint8_t>(relX, relY, relZ));
	}
	dirtyBlocks.clear();
	return res;
}

bool Chunk::isOpaqueCube(uint8_t x, uint8_t y, uint8_t z) {
	return opaqueCubeCache[blockIdx(x, y, z)];
}

void Chunk::setBlockId(uint8_t x, uint8_t y, uint8_t z, BlockId id, bool isOpaqueCube) {
	blocks[blockIdx(x, y, z)] = id;
	opaqueCubeCache[blockIdx(x, y, z)] = isOpaqueCube;
}
