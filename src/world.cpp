#include "world.hpp"

uint64_t getChunkId(int32_t x, int32_t z) {
	uint32_t x2 = (uint32_t) x;
	uint32_t z2 = (uint32_t) z;
	return ((uint64_t) x2) << 32 | ((uint64_t) z2);
}

World::World() { }

std::pair<int32_t,int32_t> World::getChunkAt(int32_t x, int32_t z) {
	int32_t chunkX = floor(((float) x) / CHUNK_SIZE);
	int32_t chunkZ = floor(((float) z) / CHUNK_SIZE);
	return std::pair<int32_t,int32_t>(chunkX, chunkZ);
}

bool World::isChunkLoaded(int32_t x, int32_t z) {
	return loadedChunks.count(getChunkId(x, z)) == 1;
}

Chunk& World::getChunk(int32_t x, int32_t z) {
	return loadedChunks.at(getChunkId(x, z));
}

Chunk& World::genChunk(int32_t x, int32_t z) {
	uint64_t key = getChunkId(x, z);
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
	Chunk& chunk = loadedChunks[getChunkId(chunkX, chunkZ)];
	int32_t relX = x - CHUNK_SIZE*chunkX;
	int32_t relZ = z - CHUNK_SIZE*chunkZ;
	return chunk.setBlock(relX, y, relZ, block);
}

void World::removeBlock(int32_t x, int32_t y, int32_t z) {
	int chunkX, chunkZ;
	std::tie(chunkX, chunkZ) = getChunkAt(x, z);
	Chunk& chunk = loadedChunks[getChunkId(chunkX, chunkZ)];
	int32_t relX = x - CHUNK_SIZE*chunkX;
	int32_t relZ = z - CHUNK_SIZE*chunkZ;
	return chunk.removeBlock(relX, y, relZ);
}

std::tuple<bool, int,int,int> World::raycast(glm::vec3 pos, glm::vec3 dir, float maxDist, bool offset) {
	Ray ray(pos, dir);
	bool hit = hasBlock(ray.getX(), ray.getY(), ray.getZ());
	while(!hit && ray.getDistance() <= maxDist && ray.getY() < 64) {
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

std::tuple<int,int,int> World::collideHorDisk(glm::vec3 pos, float radius, float horBarrier, float verBarrier) {
	int hitX = 0;
	int hitY = 0;
	int hitZ = 0;
	int blockX, blockY, blockZ;
	std::tie(blockX, blockY, blockZ) = getBlockCoordsAt(pos);
	bool inBlock = hasBlock(blockX, blockY, blockZ);
	float relX = pos.x - blockX; // [-0.5, 0.5]
	float relY = pos.y - blockY;
	float relZ = pos.z - blockZ;
	if(    (inBlock && relX <= -0.5 + horBarrier)
		|| (hasBlock(blockX+1, blockY, blockZ) && intervalIntersect(pos.x, pos.x + radius, blockX+0.5, blockX+0.5+horBarrier))) {
		hitX--;
	}
	if(    (inBlock && relX >=  0.5 - horBarrier)
		|| (hasBlock(blockX-1, blockY, blockZ) && intervalIntersect(pos.x - radius, pos.x, blockX-0.5-horBarrier, blockX-0.5))) {
		hitX++;
	}
	if(    (inBlock && relZ <= -0.5 + horBarrier)
		|| (hasBlock(blockX, blockY, blockZ+1) && intervalIntersect(pos.z, pos.z + radius, blockZ+0.5, blockZ+0.5+horBarrier))) {
		hitZ--;
	}
	if(    (inBlock && relX >=  0.5 - horBarrier)
		|| (hasBlock(blockX, blockY, blockZ-1) && intervalIntersect(pos.z - radius, pos.z, blockZ-0.5-horBarrier, blockZ-0.5))) {
		hitZ++;
	}
	if(inBlock) {
		if(relY <= -0.5 + verBarrier) {
			hitY--;
		} else if(relY >= 0.5 - verBarrier) {
			hitY++;
		}
	}
	return std::tuple<int,int,int>(hitX, hitY, hitZ);
}
