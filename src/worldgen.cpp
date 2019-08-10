#include "worldgen.hpp"

WorldGenerator::WorldGenerator()
	: seed(WorldGenerator::newSeed()), noiseGen((int64_t) seed) { }

void WorldGenerator::generateChunk(Chunk& chunk, int32_t chunkX, int32_t chunkZ) {
	BlockId stoneId = 1;
	BlockId dirtId = 2;
	BlockId grassId = 3;
	for(uint8_t relX = 0; relX < CHUNK_SIZE; relX++) {
		for(uint8_t relZ = 0; relZ < CHUNK_SIZE; relZ++) {
			int32_t x = chunkX*CHUNK_SIZE + relX;
			int32_t z = chunkZ*CHUNK_SIZE + relZ;
			uint8_t h = getTerrainHeight(x, z);
			for(uint8_t y = 0; y < h - 1; y++) {
				chunk.setBlockId(relX, y, relZ, stoneId, true);
			}
			chunk.setBlockId(relX, h-1, relZ, dirtId, true);
			chunk.setBlockId(relX, h, relZ, grassId, true);
		}
	}
	generateTree(chunk, chunkX, chunkZ, CHUNK_SIZE/2, CHUNK_SIZE/2);
}

uint64_t WorldGenerator::newSeed() {
	/*
	uint64_t a = (uint64_t) time(nullptr);
	std::random_device rand;
	uint64_t b = ((uint64_t) rand()) << 32 | ((uint64_t) rand());
	return wyhash64(a, b);
	*/
	return 42;
}

uint8_t WorldGenerator::getTerrainHeight(int32_t x, int32_t z) {
	return 28 + floor(8 * noiseGen.Evaluate(x / 20.0, z / 20.0));
}

void WorldGenerator::generateTree(Chunk& chunk, int32_t chunkX, int32_t chunkZ, int8_t rootX, int8_t rootZ) {
	uint8_t h = getTerrainHeight(chunkX*CHUNK_SIZE + rootX, chunkZ*CHUNK_SIZE + rootZ) + 1;
	for(int y = h; y <= h + 3; ++y) {
		chunk.setBlockId(rootX, y, rootZ, 4, true);
	}
	for(int y = h + 2; y <= h + 4; ++y) {
		for(uint8_t relX = rootX - 2; relX <= rootX + 2; ++relX) {
			for(uint8_t relZ = rootZ - 2; relZ <= rootZ + 2; ++relZ) {
				int d = abs(relX - rootX) + abs(relZ - rootZ);
				bool place = (y <= h + 3) ? (d >= 1 && d <= 3) : (d <= 1);
				if(place) {
					chunk.setBlockId(relX, y, relZ, 5, true);
				}
			}
		}
	}
}
