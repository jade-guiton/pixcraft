#include "worldgen.hpp"

#include <cmath>

#include "blocks.hpp"
#include "random.hpp"

WorldGenerator::WorldGenerator()
	: seed(generateSeed()), terrainHeightNoise(getFeatureSeed(seed, FeatureType::terrainHeight)) { }

void WorldGenerator::generateChunk(Chunk& chunk, int32_t chunkX, int32_t chunkZ) {
	for(uint8_t relX = 0; relX < CHUNK_SIZE; ++relX) {
		for(uint8_t relZ = 0; relZ < CHUNK_SIZE; ++relZ) {
			int32_t x = chunkX*CHUNK_SIZE + relX;
			int32_t z = chunkZ*CHUNK_SIZE + relZ;
			uint8_t h = getTerrainHeight(x, z);
			for(uint8_t y = 0; y < h - 1; ++y) {
				chunk.setBlockId(relX, y, relZ, BlockRegistry::STONE_ID, true);
			}
			if(h >= WATER_LEVEL) {
				chunk.setBlockId(relX, h-1, relZ, BlockRegistry::DIRT_ID, true);
				chunk.setBlockId(relX, h, relZ, BlockRegistry::GRASS_ID, true);
			} else {
				chunk.setBlockId(relX, h-1, relZ, BlockRegistry::DIRT_ID, true);
				chunk.setBlockId(relX, h, relZ, BlockRegistry::DIRT_ID, true);
				for(uint8_t y = h+1; y <= WATER_LEVEL; ++y) {
					chunk.setBlockId(relX, y, relZ, BlockRegistry::WATER_ID, false);
				}
			}
		}
	}
	
	std::vector<float> trees = distributeObjects(getFeatureSeed(seed, FeatureType::trees),
		chunkX*CHUNK_SIZE - 0.5, chunkZ*CHUNK_SIZE - 0.5, CHUNK_SIZE, 6, 2.5);
	for(size_t i = 0; i < trees.size(); i += 2) {
		int32_t x = round(trees[i]);
		int32_t z = round(trees[i + 1]);
		int32_t relX = x - chunkX*CHUNK_SIZE;
		int32_t relZ = z - chunkZ*CHUNK_SIZE;
		generateTree(chunk, chunkX, chunkZ, relX, relZ);
	}
}

uint8_t WorldGenerator::getTerrainHeight(int32_t x, int32_t z) {
	return 32 + round(8 * terrainHeightNoise.Evaluate(x / 20.0, z / 20.0));
}

void WorldGenerator::generateTree(Chunk& chunk, int32_t chunkX, int32_t chunkZ, int8_t rootX, int8_t rootZ) {
	uint8_t h = getTerrainHeight(chunkX*CHUNK_SIZE + rootX, chunkZ*CHUNK_SIZE + rootZ) + 1;
	if(h <= WATER_LEVEL) return;
	if(!INVALID_BLOCK_POS(rootX, 0, rootZ)) {
		for(int y = h; y <= h + 3; ++y) {
			chunk.setBlockId(rootX, y, rootZ, BlockRegistry::TRUNK_ID, true);
		}
	}
	for(int y = h + 2; y <= h + 4; ++y) {
		for(int8_t relX = rootX - 2; relX <= rootX + 2; ++relX) {
			for(int8_t relZ = rootZ - 2; relZ <= rootZ + 2; ++relZ) {
				if(!INVALID_BLOCK_POS(relX, y, relZ)) {
					int d = abs(relX - rootX) + abs(relZ - rootZ);
					bool place = (y <= h + 3) ? (d >= 1 && d <= 3) : (d <= 1);
					if(place) {
						chunk.setBlockId(relX, y, relZ, BlockRegistry::LEAVES_ID, false);
					}
				}
			}
		}
	}
}
