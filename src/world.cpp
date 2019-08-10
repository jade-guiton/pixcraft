#include "world.hpp"

World::World() { }

bool World::isValidHeight(int32_t y) {
	return 0 <= y && y < CHUNK_HEIGHT;
}

std::pair<int32_t,int32_t> World::getChunkAt(int32_t x, int32_t z) {
	int32_t chunkX = floor(((float) x) / CHUNK_SIZE);
	int32_t chunkZ = floor(((float) z) / CHUNK_SIZE);
	return std::pair<int32_t,int32_t>(chunkX, chunkZ);
}

bool World::isChunkLoaded(int32_t x, int32_t z) {
	return loadedChunks.count(packCoords(x, z)) == 1;
}

Chunk& World::getChunk(int32_t x, int32_t z) {
	return loadedChunks.at(packCoords(x, z));
}

Chunk& World::genChunk(int32_t x, int32_t z) {
	uint64_t key = packCoords(x, z);
	loadedChunks.erase(key);
	gen.generateChunk(loadedChunks[key], x, z);
	return loadedChunks.at(key);
}

bool World::hasBlock(int32_t x, int32_t y, int32_t z) {
	int chunkX, chunkZ;
	std::tie(chunkX, chunkZ) = getChunkAt(x, z);
	if(!isChunkLoaded(chunkX, chunkZ)) return false;
	Chunk& chunk = getChunk(chunkX, chunkZ);
	int32_t relX = x - CHUNK_SIZE*chunkX;
	int32_t relZ = z - CHUNK_SIZE*chunkZ;
	return chunk.hasBlock(relX, y, relZ);
}

Block* World::getBlock(int32_t x, int32_t y, int32_t z) {
	int chunkX, chunkZ;
	std::tie(chunkX, chunkZ) = getChunkAt(x, z);
	if(!isChunkLoaded(chunkX, chunkZ)) return nullptr;
	Chunk& chunk = getChunk(chunkX, chunkZ);
	int32_t relX = x - CHUNK_SIZE*chunkX;
	int32_t relZ = z - CHUNK_SIZE*chunkZ;
	return chunk.getBlock(relX, y, relZ);
}

void World::setBlock(int32_t x, int32_t y, int32_t z, Block& block) {
	int chunkX, chunkZ;
	std::tie(chunkX, chunkZ) = getChunkAt(x, z);
	Chunk& chunk = loadedChunks[packCoords(chunkX, chunkZ)];
	int32_t relX = x - CHUNK_SIZE*chunkX;
	int32_t relZ = z - CHUNK_SIZE*chunkZ;
	return chunk.setBlock(relX, y, relZ, block);
}

void World::removeBlock(int32_t x, int32_t y, int32_t z) {
	int chunkX, chunkZ;
	std::tie(chunkX, chunkZ) = getChunkAt(x, z);
	if(!isChunkLoaded(chunkX, chunkZ)) return;
	Chunk& chunk = loadedChunks[packCoords(chunkX, chunkZ)];
	int32_t relX = x - CHUNK_SIZE*chunkX;
	int32_t relZ = z - CHUNK_SIZE*chunkZ;
	return chunk.removeBlock(relX, y, relZ);
}

std::tuple<bool, int,int,int> World::raycast(glm::vec3 pos, glm::vec3 dir, float maxDist, bool offset) {
	Ray ray(pos, dir);
	bool hit = hasBlock(ray.getX(), ray.getY(), ray.getZ());
	while(!hit && ray.getDistance() <= maxDist) {
		ray.nextFace();
		hit = hasBlock(ray.getX(), ray.getY(), ray.getZ());
	}
	if(hit) {
		int32_t x = ray.getX();
		int32_t y = ray.getY();
		int32_t z = ray.getZ();
		if(offset) {
			int face = ray.getLastFace();
			x += sideVectors[face][0];
			y += sideVectors[face][1];
			z += sideVectors[face][2];
		}
		return std::tuple<bool, int,int,int>(true, x, y, z);
	} else {
		return std::tuple<bool, int,int,int>(false, 0, 0, 0);
	}
}

bool World::hasBlocksInLine(int x, int z, float base, float height) {
	int y1 = getBlockCoordAt(base);
	int y2 = getBlockCoordAt(base + height);
	for(int y = y1; y <= y2; ++y) {
		if(hasBlock(x, y, z)) return true;
	}
	return false;
}

glm::vec2 World::collideCylHor(glm::vec3 center, float radius, float height, float margin) {
	int blockX, blockY, blockZ;
	std::tie(blockX, blockY, blockZ) = getBlockCoordsAt(center);
	
	float relX = center.x - blockX; // [-0.5, 0.5]
	float relZ = center.z - blockZ;
	
	if(!hasBlocksInLine(blockX, blockZ, center.y, height)) {
		int dirX = (relX >= 0.5 - radius) - (relX <= -0.5 + radius); // are we overlapping with a neighbor cell, and which
		int dirZ = (relZ >= 0.5 - radius) - (relZ <= -0.5 + radius);
		bool collideX = dirX != 0 && hasBlocksInLine(blockX + dirX, blockZ, center.y, height); // are we colliding with a neighbor block
		bool collideZ = dirZ != 0 && hasBlocksInLine(blockX, blockZ + dirZ, center.y, height);
		
		if(collideX || collideZ) {
			glm::vec2 disp(0);
			if(collideX)
				disp.x = dirX*(0.5 - radius - margin) - relX;
			if(collideZ)
				disp.y = dirZ*(0.5 - radius - margin) - relZ;
			return disp;
		} else if(dirX != 0 && dirZ != 0 && hasBlocksInLine(blockX + dirX, blockZ + dirZ, center.y, height)) { // potentially colliding with diagonal block
			glm::vec2 horCenter(center.x, center.z);
			glm::vec2 corner(blockX + dirX*0.5, blockZ + dirZ*0.5);
			float dist(glm::length(horCenter-corner));
			if(dist <= radius) {
				if(dist < 0.01) { // if somehow we're reaaaally close to the corner
					float sideLen(radius / sqrt(2.0) + margin);
					return glm::vec2(-dirX*sideLen, -dirZ*sideLen);
				} else {
					return (horCenter-corner) * ((radius - dist + margin) / dist);
				}
			}
		}
	} else { // useful if we're going reaaaally fast
		const float coreSize = 0.25;
		
		int dirX = (relX >= 0) - (relX < 0); // what part of the cell are we in
		int dirZ = (relZ >= 0) - (relZ < 0);
		int inBarX = std::abs(relX) >= 0.5 - coreSize;
		int inBarZ = std::abs(relZ) >= 0.5 - coreSize;
		
		if(inBarX || inBarZ) {
			bool blocksOnX = hasBlocksInLine(blockX + dirX, blockZ, center.y, height);
			bool blocksOnZ = hasBlocksInLine(blockX, blockZ + dirZ, center.y, height);
			
			bool preferX = inBarX && !inBarZ;
			bool preferZ = inBarZ && !inBarX;
			if(!preferX && !preferZ) { // we're in a corner
				// see if there's another reasonable preference for pushing towards X or Z:
				bool closerToX = std::abs(relX) > std::abs(relZ);
				preferX = closerToX && !blocksOnX;
				preferZ = !closerToX && !blocksOnZ;
				if(!preferX && !preferZ) { // otherwise...
					preferX = !blocksOnX;
					preferZ = !blocksOnZ;
					if((preferX && preferZ) || (!preferX && !preferZ)) { // even otherwise...
						preferX = closerToX;
					}
				}
			}
			
			if(preferX)
				return glm::vec2(dirX*(0.5 + radius) - relX, 0);
			else
				return glm::vec2(0, dirZ*(0.5 + radius) - relZ);
		} else {
			return glm::vec2(0);
		}
		
	}
	return glm::vec2(0);
}

float World::collideDiskVer(glm::vec3 center, float radius, float verBarrier, float margin) {
	int blockX, blockY, blockZ;
	std::tie(blockX, blockY, blockZ) = getBlockCoordsAt(center);
	
	float relX = center.x - blockX; // [-0.5, 0.5]
	float relY = center.y - blockY;
	float relZ = center.z - blockZ;
	
	bool inBlock = hasBlock(blockX, blockY, blockZ);
	if(!inBlock) { // ADVANCED collision detection
		int dirX = (relX >= 0.5 - radius) - (relX <= -0.5 + radius); // are we overlapping with a neighbor cell, and which
		int dirZ = (relZ >= 0.5 - radius) - (relZ <= -0.5 + radius);
		if(dirX != 0)
			inBlock = inBlock || hasBlock(blockX + dirX, blockY, blockZ);
		if(dirZ != 0)
			inBlock = inBlock || hasBlock(blockX, blockY, blockZ + dirZ);
		if(!inBlock && dirX != 0 && dirZ != 0 && hasBlock(blockX + dirX, blockY, blockZ + dirZ)) { // ＡＤＶＡＮＣＥＤＥＲ
			glm::vec2 horCenter(center.x, center.z);
			glm::vec2 corner(blockX + dirX*0.5, blockZ + dirZ*0.5);
			inBlock = glm::length(horCenter-corner) <= radius;
		}
	}
	if(inBlock) {
		if(relY >= 0.5 - verBarrier) {
			return 0.5 - relY + margin;
		} else if(relY <= -0.5 + verBarrier) {
			return -0.5 - relY - margin;
		}
	}
	return 0.0;
}

