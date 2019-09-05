#include "world.hpp"

#include <cmath>
#include <fstream>

#include "blocks.hpp"
#include "mob.hpp"

#include "../serializer_generated.h"

using namespace PixCraft;

World::World() { }

void World::saveToFile() {
	flatbuffers::FlatBufferBuilder builder;
	
	std::vector<flatbuffers::Offset<Serializer::Chunk>> chunkOffsets;
	for(auto& pair : loadedChunks) {
		int32_t chunkX, chunkZ;
		std::tie(chunkX, chunkZ) = unpackCoords(pair.first);
		chunkOffsets.push_back(pair.second.serialize(chunkX, chunkZ, builder));
	}
	auto chunkVector = builder.CreateVector(chunkOffsets);
	
	std::vector<flatbuffers::Offset<void>> mobOffsets;
	std::vector<uint8_t> mobTypes;
	for(auto& mobPointer : mobs) {
		mobOffsets.push_back(mobPointer->serialize(builder));
		mobTypes.push_back(mobPointer->serializedType());
	}
	auto mobVector = builder.CreateVector(mobOffsets);
	auto mobTypeVector = builder.CreateVector(mobTypes);
	auto world = Serializer::CreateWorld(builder, chunkVector, mobTypeVector, mobVector);
	
	builder.Finish(world);
	std::ofstream file("data/world.bin", std::ios::binary);
	uint8_t* buf = builder.GetBufferPointer();
	file.write(reinterpret_cast<const char*>(buf), builder.GetSize());
	file.close();
}

bool World::isValidHeight(int32_t y) {
	return 0 <= y && y < CHUNK_HEIGHT;
}

std::pair<int32_t,int32_t> World::getChunkPosAt(int32_t x, int32_t z) {
	int32_t chunkX = floor(((float) x) / CHUNK_SIZE);
	int32_t chunkZ = floor(((float) z) / CHUNK_SIZE);
	return std::pair<int32_t,int32_t>(chunkX, chunkZ);
}

uint64_t World::getChunkIdxAt(int32_t x, int32_t z) {
	int32_t chunkX, chunkZ;
	std::tie(chunkX, chunkZ) = getChunkPosAt(x, z);
	return packCoords(chunkX, chunkZ);
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
	Chunk& chunk = loadedChunks[key];
	chunk.init(this);
	gen.generateChunk(chunk, x, z);
	return chunk;
}

std::tuple<Chunk*, uint8_t, uint8_t> World::getBlockFromChunk(int32_t x, int32_t z) {
	int chunkX, chunkZ;
	std::tie(chunkX, chunkZ) = getChunkPosAt(x, z);
	int32_t relX = x - CHUNK_SIZE*chunkX;
	int32_t relZ = z - CHUNK_SIZE*chunkZ;
	if(isChunkLoaded(chunkX, chunkZ)) {
		return std::tuple<Chunk*, uint8_t, uint8_t>(&getChunk(chunkX, chunkZ), relX, relZ);
	} else {
		return std::tuple<Chunk*, uint8_t, uint8_t>(nullptr, relX, relZ);
	}
}

void World::markDirty(int32_t x, int32_t y, int32_t z) {
	if(!isValidHeight(y)) return;
	dirtyBlocks.insert(std::tuple<int32_t, int32_t, int32_t>(x, y, z));
}

BlockPosSet World::retrieveDirtyBlocks() {
	BlockPosSet res;
	res.swap(dirtyBlocks);
	return res;
}

void World::requestUpdate(int32_t x, int32_t y, int32_t z) {
	if(!isValidHeight(y)) return;
	Chunk* chunk; int relX, relZ;
	std::tie(chunk, relX, relZ) = getBlockFromChunk(x, z);
	// TODO: if chunk doesn't exist yet, stash the update maybe?
	if(chunk == nullptr) return;
	chunk->requestUpdate(relX, y, relZ);
	updateRequests.insert(getChunkIdxAt(x, z));
}

void World::requestUpdatesAround(int32_t x, int32_t y, int32_t z) {
	for(int side = 0; side < 6; ++side) {
		requestUpdate(x + sideVectors[side][0], y + sideVectors[side][1], z + sideVectors[side][2]);
	}
}

void World::updateBlocks() {
	std::unordered_set<uint64_t> updates;
	updateRequests.swap(updates);
	for(uint64_t chunkIdx : updates) {
		int32_t chunkX, chunkZ;
		std::tie(chunkX, chunkZ) = unpackCoords(chunkIdx);
		loadedChunks[chunkIdx].updateBlocks(chunkX, chunkZ);
	}
}


bool World::hasBlock(int32_t x, int32_t y, int32_t z) {
	if(!isValidHeight(y)) return false;
	Chunk* chunk; int relX, relZ;
	std::tie(chunk, relX, relZ) = getBlockFromChunk(x, z);
	if(chunk == nullptr) return false;
	return chunk->hasBlock(relX, y, relZ);
}

Block* World::getBlock(int32_t x, int32_t y, int32_t z) {
	if(!isValidHeight(y)) return nullptr;
	Chunk* chunk; int relX, relZ;
	std::tie(chunk, relX, relZ) = getBlockFromChunk(x, z);
	if(chunk == nullptr) return nullptr;
	return chunk->getBlock(relX, y, relZ);
}

void World::setBlock(int32_t x, int32_t y, int32_t z, Block& block) {
	if(!isValidHeight(y)) return;
	Chunk* chunk; int relX, relZ;
	std::tie(chunk, relX, relZ) = getBlockFromChunk(x, z);
	if(chunk == nullptr) return;
	chunk->setBlock(relX, y, relZ, block);
	markDirty(x, y, z);
	requestUpdate(x, y, z);
	requestUpdatesAround(x, y, z);
}

void World::removeBlock(int32_t x, int32_t y, int32_t z) {
	if(!isValidHeight(y)) return;
	Chunk* chunk; int relX, relZ;
	std::tie(chunk, relX, relZ) = getBlockFromChunk(x, z);
	if(chunk == nullptr) return;
	chunk->removeBlock(relX, y, relZ);
	markDirty(x, y, z);
	requestUpdatesAround(x, y, z);
}

bool World::isOpaqueCube(int32_t x, int32_t y, int32_t z) {
	if(!isValidHeight(y)) return false;
	Chunk* chunk; int relX, relZ;
	std::tie(chunk, relX, relZ) = getBlockFromChunk(x, z);
	if(chunk == nullptr) return false;
	return chunk->isOpaqueCube(relX, y, relZ);
}

std::tuple<bool, int,int,int> World::raycast(glm::vec3 pos, glm::vec3 dir, float maxDist, bool offset, bool hitFluids) {
	Ray ray(pos, dir);
	bool hit = hitFluids ? hasBlock(ray.getX(), ray.getY(), ray.getZ())
	                     : hasSolidBlock(ray.getX(), ray.getY(), ray.getZ());
	while(!hit && ray.getDistance() <= maxDist) {
		ray.nextFace();
		hit = hitFluids ? hasBlock(ray.getX(), ray.getY(), ray.getZ())
	                    : hasSolidBlock(ray.getX(), ray.getY(), ray.getZ());;
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

bool World::hasSolidBlock(int32_t x, int32_t y, int32_t z) {
	Block* block = getBlock(x, y, z);
	return block != nullptr && block->collision() == BlockCollision::solidCube;
}

bool World::hasSolidBlocksInLine(int x, int z, float base, float height) {
	int y1 = getBlockCoordAt(base);
	int y2 = getBlockCoordAt(base + height);
	for(int y = y1; y <= y2; ++y) {
		if(hasSolidBlock(x, y, z)) return true;
	}
	return false;
}

glm::vec2 World::collideCylHor(glm::vec3 center, float radius, float height, float margin) {
	int blockX, blockY, blockZ;
	std::tie(blockX, blockY, blockZ) = getBlockCoordsAt(center);
	
	float relX = center.x - blockX; // [-0.5, 0.5]
	float relZ = center.z - blockZ;
	
	if(!hasSolidBlocksInLine(blockX, blockZ, center.y, height)) { // if we're not inside a block
		int dirX = (relX >= 0.5 - radius) - (relX <= -0.5 + radius); // are we overlapping with a neighbor cell, and which
		int dirZ = (relZ >= 0.5 - radius) - (relZ <= -0.5 + radius);
		bool collideX = dirX != 0 && hasSolidBlocksInLine(blockX + dirX, blockZ, center.y, height); // are we colliding with a neighbor block
		bool collideZ = dirZ != 0 && hasSolidBlocksInLine(blockX, blockZ + dirZ, center.y, height);
		
		if(collideX || collideZ) {
			glm::vec2 disp(0);
			if(collideX)
				disp.x = dirX*(0.5 - radius - margin) - relX;
			if(collideZ)
				disp.y = dirZ*(0.5 - radius - margin) - relZ;
			return disp;
		} else if(dirX != 0 && dirZ != 0 && hasSolidBlocksInLine(blockX + dirX, blockZ + dirZ, center.y, height)) { // potentially colliding with diagonal block
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
			bool blocksOnX = hasSolidBlocksInLine(blockX + dirX, blockZ, center.y, height);
			bool blocksOnZ = hasSolidBlocksInLine(blockX, blockZ + dirZ, center.y, height);
			
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
	
	bool inBlock = hasSolidBlock(blockX, blockY, blockZ);
	if(!inBlock) { // ADVANCED collision detection
		int dirX = (relX >= 0.5 - radius) - (relX <= -0.5 + radius); // are we overlapping with a neighbor cell, and which
		int dirZ = (relZ >= 0.5 - radius) - (relZ <= -0.5 + radius);
		if(dirX != 0)
			inBlock = inBlock || hasSolidBlock(blockX + dirX, blockY, blockZ);
		if(dirZ != 0)
			inBlock = inBlock || hasSolidBlock(blockX, blockY, blockZ + dirZ);
		if(!inBlock && dirX != 0 && dirZ != 0 && hasSolidBlock(blockX + dirX, blockY, blockZ + dirZ)) { // ＡＤＶＡＮＣＥＤＥＲ
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

bool World::containsMobs(int32_t x, int32_t y, int32_t z) {
	for(auto it = mobs.begin(); it != mobs.end(); ++it) {
		if((*it)->isInsideBlock(x, y, z)) return true;
	}
	return false;
}

void World::updateEntities(float dt) {
	for(auto it = mobs.begin(); it != mobs.end(); ++it) {
		(*it)->update(dt);
	}
}
