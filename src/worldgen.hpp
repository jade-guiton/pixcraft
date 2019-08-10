#pragma once

#include <cstdint>

#include "OpenSimplexNoise.hpp"

#include "chunk.hpp"

class WorldGenerator {
public:
	WorldGenerator();
	
	void generateChunk(Chunk& chunk, int32_t chunkX, int32_t chunkZ);

private:
	uint64_t seed;
	OpenSimplexNoise terrainHeightNoise;
	
	static const uint8_t WATER_LEVEL = 29;
	
	uint8_t getTerrainHeight(int32_t x, int32_t z);
	
	void generateTree(Chunk& chunk, int32_t chunkX, int32_t chunkZ, int8_t rootX, int8_t rootZ);
};
