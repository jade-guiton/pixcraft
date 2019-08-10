#pragma once

#include <unordered_map>
#include <utility>
#include <tuple>
#include <vector>
#include <cmath>

#include "glm.hpp"

#include "chunk.hpp"
#include "worldgen.hpp"
#include "blocks.hpp"
#include "util.hpp"

class World;
#include "player.hpp"

class World {
public:
	World();
	
	static bool isValidHeight(int32_t y);
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
	
	// tests if a vertical line collides with blocks
	bool hasBlocksInLine(int x, int z, float y1, float height);
	
	// tests a cylinder with base (center, radius) for horizontal collision with blocks
	// returns a horizontal vector, such that moving the cylinder by that vector would stop the horizontal collision
	// constraint: radius < 0.5
	glm::vec2 collideCylHor(glm::vec3 center, float radius, float height, float margin);
	
	// tests a given horizontal disk for vertical collision with blocks
	// returns the vertical algebraic distance in which to move to stop the vertical collision
	// verBarrier determines how far the center can venture inside a block for the collision to continue to be acknowledged
	float collideDiskVer(glm::vec3 center, float radius, float verBarrier, float margin);
	
	std::vector<Player> players;
	
private:
	WorldGenerator gen;
	std::unordered_map<uint64_t, Chunk> loadedChunks;
};
