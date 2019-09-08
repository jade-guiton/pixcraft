#pragma once

#include <cstdint>

#include "pixcraft/util/OpenSimplexNoise.hpp"

#include "world_module.hpp"

namespace PixCraft {
	class WorldGenerator {
	public:
		WorldGenerator(uint64_t seed);
		WorldGenerator();
		
		uint64_t seed();
		
		void generateChunk(Chunk& chunk, int32_t chunkX, int32_t chunkZ);

	private:
		uint64_t _seed;
		OpenSimplexNoise terrainHeightNoise;
		
		static const uint8_t WATER_LEVEL = 30;
		
		uint8_t getTerrainHeight(int32_t x, int32_t z);
		
		void generateTree(Chunk& chunk, int32_t chunkX, int32_t chunkZ, int8_t rootX, int8_t rootZ);
	};
}
