#pragma once

#include <cstdint>

#include "blocks.hpp"
#include "chunk.hpp"

class WorldGenerator {
public:
	WorldGenerator();
	
	void generateChunk(Chunk& chunk, int32_t chunkX, int32_t chunkZ);
};
