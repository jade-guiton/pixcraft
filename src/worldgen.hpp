#pragma once

#include <cstdint>
#include <cmath>
#include <ctime>
#include <random>

#include "blocks.hpp"
#include "chunk.hpp"
#include "OpenSimplexNoise.hpp"
#include "wyhash.h"

class WorldGenerator {
public:
	WorldGenerator();
	
	void generateChunk(Chunk& chunk, int32_t chunkX, int32_t chunkZ);

private:
	uint64_t seed;
	OpenSimplexNoise noiseGen;
	
	static uint64_t newSeed();
	
	uint8_t getTerrainHeight(int32_t x, int32_t z);
	
	void generateTree(Chunk& chunk, int32_t chunkX, int32_t chunkZ, int8_t relX, int8_t relY);
};
