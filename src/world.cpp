#include "world.hpp"

uint64_t getChunkId(int32_t x, int32_t z) {
	uint32_t x2 = (uint32_t) x;
	uint32_t z2 = (uint32_t) z;
	return ((uint64_t) x2) << 32 | ((uint64_t) z2);
}

World::World()
	: playerPos(8.0f, 40.0f, 8.0f), playerOrient(0.0f, 0.0f, 0.0f) { }

std::pair<int32_t,int32_t> World::getChunkAt(int32_t x, int32_t z) {
	int32_t chunkX = floor(((float) x) / CHUNK_SIZE);
	int32_t chunkZ = floor(((float) z) / CHUNK_SIZE);
	return std::pair<int32_t,int32_t>(chunkX, chunkZ);
}

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

Block* World::getBlock(int32_t x, int32_t y, int32_t z) {
	int chunkX, chunkZ;
	std::tie(chunkX, chunkZ) = getChunkAt(x, z);
	if(!isChunkLoaded(chunkX, chunkZ)) return nullptr;
	Chunk& chunk = getChunk(chunkX, chunkZ);
	int32_t relX = x - CHUNK_SIZE*chunkX;
	int32_t relZ = z - CHUNK_SIZE*chunkZ;
	return chunk.getBlock(relX, y, relZ);
}

void World::setBlock(int32_t x, int32_t y, int32_t z, Block& block) {
	int chunkX, chunkZ;
	std::tie(chunkX, chunkZ) = getChunkAt(x, z);
	Chunk& chunk = loadedChunks[getChunkId(chunkX, chunkZ)];
	int32_t relX = x - CHUNK_SIZE*chunkX;
	int32_t relZ = z - CHUNK_SIZE*chunkZ;
	return chunk.setBlock(relX, y, relZ, block);
}

void World::removeBlock(int32_t x, int32_t y, int32_t z) {
	int chunkX, chunkZ;
	std::tie(chunkX, chunkZ) = getChunkAt(x, z);
	Chunk& chunk = loadedChunks[getChunkId(chunkX, chunkZ)];
	int32_t relX = x - CHUNK_SIZE*chunkX;
	int32_t relZ = z - CHUNK_SIZE*chunkZ;
	return chunk.removeBlock(relX, y, relZ);
}
