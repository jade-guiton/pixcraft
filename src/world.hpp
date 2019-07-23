#pragma once

#include <unordered_map>
#include <utility>
#include <tuple>
#include <vector>

#include "glm.hpp"

#include "chunk.hpp"
#include "worldgen.hpp"
#include "blocks.hpp"
#include "util.hpp"

class World;
#include "player.hpp"

uint64_t getChunkId(int32_t x, int32_t z);

class World {
public:
	World();
	
	static std::pair<int32_t, int32_t> getChunkAt(int32_t x, int32_t z);
	
	bool isChunkLoaded(int32_t x, int32_t z);
	Chunk& getChunk(int32_t x, int32_t z);
	Chunk& genChunk(int32_t x, int32_t z);
	
	bool hasBlock(int32_t x, int32_t y, int32_t z);
	Block* getBlock(int32_t x, int32_t y, int32_t z);
	void setBlock(int32_t x, int32_t y, int32_t z, Block& block);
	void removeBlock(int32_t x, int32_t y, int32_t z);
	
	// sends a ray from pos in dir, on maxDist, and tests for block collisions; returns a tuple with:
	// { hit?, hitBlockX, hitBlockY, hitBlockZ }
	// if offset, the air block before the hit block is returned instead.
	std::tuple<bool, int,int,int> raycast(glm::vec3 pos, glm::vec3 dir, float maxDist, bool offset);
	
	// tests a horizontal disk with center pos and radius for collision with blocks; returns:
	// { collideX, collideY, collideZ }
	// for each coordinate, the return value is -1, 0, 1 corresponding to the direction in which the
	// disk should move to stop colliding with the block. If the circle isn't colliding along this axis,
	// or is colliding in two directions, 0 is returned.
	// barrierSize corresponds to the "thickness" of the surface of a block; if the center is past the
	// barrier, it is considered completely inside the block, and no collision is reported.
	// Constraints: radius < 0.5, barrierSize < 0.5
	std::tuple<int,int,int> collideHorDisk(glm::vec3 pos, float radius, float horBarrier, float verBarrier);
	
	std::vector<Player> players;
	
private:
	WorldGenerator gen;
	std::unordered_map<uint64_t, Chunk> loadedChunks;
};
