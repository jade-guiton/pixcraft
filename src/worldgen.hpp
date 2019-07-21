#pragma once

#include <cstdint>
#include <cmath>

#include "blocks.hpp"
#include "chunk.hpp"
#include "OpenSimplexNoise.hpp"

class WorldGenerator {
public:
	WorldGenerator();
	
	void generateChunk(Chunk& chunk, int32_t chunkX, int32_t chunkZ);

private:
	OpenSimplexNoise noiseGen;
};
