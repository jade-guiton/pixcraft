#include "worldgen.hpp"

WorldGenerator::WorldGenerator() { }

void WorldGenerator::generateChunk(Chunk& chunk, int32_t chunkX, int32_t chunkZ) {
	StoneBlock stone; DirtBlock dirt; GrassBlock grass;
	for(uint8_t relX = 0; relX < CHUNK_SIZE; relX++) {
		for(uint8_t relZ = 0; relZ < CHUNK_SIZE; relZ++) {
			int32_t x = chunkX*CHUNK_SIZE + relX;
			int32_t z = chunkZ*CHUNK_SIZE + relZ;
			uint8_t h = 28 + floor(8 * noiseGen.Evaluate(x / 20.0, z / 20.0));
			for(uint8_t y = 0; y < h - 1; y++) {
				chunk.setBlock(relX, y, relZ, stone);
			}
			chunk.setBlock(relX, h-1, relZ, dirt);
			chunk.setBlock(relX, h, relZ, grass);
		}
	}
}
