#include "world.hpp"

uint64_t getChunkId(int32_t x, int32_t z) {
	uint32_t x2 = (uint32_t) x;
	uint32_t z2 = (uint32_t) z;
	return ((uint64_t) x2) << 32 | ((uint64_t) z2);
}

World::World()
	: playerPos(8.0f, 40.0f, 8.0f), playerOrient(0.0f, 0.0f, 0.0f) { }

bool World::isChunkLoaded(int32_t x, int32_t z) {
	return loadedChunks.count(getChunkId(x, z)) == 1;
}

Chunk& World::getChunk(int32_t x, int32_t z) {
	return loadedChunks.at(getChunkId(x, z));
}

Chunk& World::genChunk(int32_t x, int32_t z) {
	uint64_t key = getChunkId(x, z);
	loadedChunks.erase(key);
	gen.generateChunk(loadedChunks[key], x, z);
	return loadedChunks.at(key);
}
