#pragma once

#include <cstdint>

#include "world.hpp"
#include "blocks.hpp"

class WorldGenerator {
public:
	WorldGenerator();
	
	void generateChunk(Chunk& chunk, int32_t chunkX, int32_t chunkY, int32_t chunkZ);
};
