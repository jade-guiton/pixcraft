#include "worldgen.hpp"

WorldGenerator::WorldGenerator() { }

void WorldGenerator::generateChunk(Chunk& chunk, int32_t chunkX, int32_t chunkZ) {
	StoneBlock stone; DirtBlock dirt; GrassBlock grass;
	for(uint8_t x = 0; x < CHUNK_SIZE; x++) {
		for(uint8_t z = 0; z < CHUNK_SIZE; z++) {
			chunk.setBlock(x, 0, z, stone);
			chunk.setBlock(x, 1, z, dirt);
			chunk.setBlock(x, 2, z, grass);
		}
	}
}
