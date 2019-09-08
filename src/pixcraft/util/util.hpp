#pragma once

#include <string>
#include <tuple>
#include <utility>
#include <unordered_set>

#include "pixcraft/util/glm.hpp"

namespace PixCraft {
	#define TAU 6.28318531f
	
	extern const int sideVectors[6][3];
	
	typedef std::tuple<int32_t, int32_t, int32_t> BlockPos;
	
	struct BlockPosHash : public std::unary_function<BlockPos, std::size_t> {
		std::size_t operator()(const BlockPos& k) const {
			return std::get<0>(k) ^ std::get<1>(k) ^ std::get<2>(k);
		}
	};
	
	typedef std::unordered_set<BlockPos, BlockPosHash> BlockPosSet;
	
	glm::mat4 globalToLocalRot(glm::vec3 orient);
	glm::mat4 localToGlobalRot(glm::vec3 orient);
	glm::mat4 globalToLocal(glm::vec3 pos, glm::vec3 orient);
	glm::mat4 localToGlobal(glm::vec3 pos, glm::vec3 orient);
	
	std::string vec3ToString(glm::vec3 pos);
	
	class SpiralIterator {
	public:
		SpiralIterator(int startX, int startZ);
		
		int getX();
		int getZ();
		bool withinSquareDistance(int dist);
		bool withinDistance(int dist);
		
		void next();
		
	private:
		int startX, startZ;
		int x, z;
		int dx, dz;
	};
	
	
	int sign(float x);
	
	// From a world coordinate, returns the block it's inside's corresponding coordinate
	int getBlockCoordAt(float x);
	
	class Ray {
	public:
		Ray(glm::vec3 startPos, glm::vec3 dir);
		
		int getX();
		int getY();
		int getZ();
		float getDistance();
		int getLastFace();
		
		void nextFace();

	private:
		float tDeltaX, tDeltaY, tDeltaZ;
		int stepX, stepY, stepZ;
		
		int x, y, z;
		float dist;
		float tMaxX, tMaxY, tMaxZ;
		int lastFace;
	};
	
	std::tuple<int,int,int> getBlockCoordsAt(glm::vec3 pos);
	
	uint64_t packCoords(int32_t x, int32_t z);
	std::pair<int32_t, int32_t> unpackCoords(uint64_t v);
	
	// checks if a cylinder with base (center, radius) and given height collides with block at (x,y,z)
	bool cylinderBlockCollision(glm::vec3 center, float radius, float height, int32_t x, int32_t y, int32_t z);
}
