#include "chunk_renderer.hpp"

#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>
#include <utility>

#include <iostream>

#include "glm.hpp"

#include "blocks.hpp"
#include "chunk.hpp"
#include "util.hpp"

void RenderedChunk::init(FaceRenderer& faceRenderer, World& world2, int32_t chunkX2, int32_t chunkZ2) {
	buffer.init(faceRenderer, MAX_CHUNK_FACES);
	translucentBuffer.init(faceRenderer, MAX_CHUNK_FACES);
	world = &world2;
	chunkX = chunkX2; chunkZ = chunkZ2;
}

bool RenderedChunk::isInitialized() { return buffer.isInitialized(); }

void RenderedChunk::prerender() {
	Chunk& chunk = world->getChunk(chunkX, chunkZ);
	
	buffer.faces.clear();
	translucentBuffer.faces.clear();
	for(uint8_t x = 0; x < CHUNK_SIZE; ++x) {
		for(uint8_t y = 0; y < CHUNK_HEIGHT; ++y) {
			for(uint8_t z = 0; z < CHUNK_SIZE; ++z) {
				prerenderBlock(chunk, x, y, z);
			}
		}
	}
	
	buffer.prerender();
	translucentBuffer.prerender();
}

void RenderedChunk::updateBlock(int8_t relX, int8_t y, int8_t relZ) {
	buffer.eraseFaces(relX, y, relZ);
	translucentBuffer.eraseFaces(relX, y, relZ);
	
	Chunk& chunk = world->getChunk(chunkX, chunkZ);
	prerenderBlock(chunk, relX, y, relZ);
	
	buffer.prerender();
	translucentBuffer.prerender();
}

void RenderedChunk::updatePlaneX(int8_t relX) {
	buffer.erasePlaneX(relX);
	translucentBuffer.erasePlaneX(relX);
	
	Chunk& chunk = world->getChunk(chunkX, chunkZ);
	for(uint8_t y = 0; y < CHUNK_HEIGHT; ++y) {
		for(uint8_t relZ = 0; relZ < CHUNK_SIZE; ++relZ) {
			prerenderBlock(chunk, relX, y, relZ);
		}
	}
	
	buffer.prerender();
	translucentBuffer.prerender();
}

void RenderedChunk::updatePlaneZ(int8_t relZ) {
	buffer.erasePlaneZ(relZ);
	translucentBuffer.erasePlaneZ(relZ);
	
	Chunk& chunk = world->getChunk(chunkX, chunkZ);
	for(uint8_t y = 0; y < CHUNK_HEIGHT; ++y) {
		for(uint8_t relX = 0; relX < CHUNK_SIZE; ++relX) {
			prerenderBlock(chunk, relX, y, relZ);
		}
	}
	
	buffer.prerender();
	translucentBuffer.prerender();
}

void RenderedChunk::render(FaceRenderer& faceRenderer) {
	glm::mat4 model = glm::translate(glm::mat4(1.0f), ((float) CHUNK_SIZE) * glm::vec3(chunkX, 0.0f, chunkZ));
	faceRenderer.render(buffer, model);
}

void RenderedChunk::renderTranslucent(FaceRenderer& faceRenderer) {
	glm::mat4 model = glm::translate(glm::mat4(1.0f), ((float) CHUNK_SIZE) * glm::vec3(chunkX, 0.0f, chunkZ));
	faceRenderer.render(translucentBuffer, model);
}


void RenderedChunk::prerenderBlock(Chunk& chunk, int8_t relX, int8_t y, int8_t relZ) {
	Block* block = chunk.getBlock(relX, y, relZ);
	if(block == nullptr) return;
	
	for(uint8_t side = 0; side < 6; ++side) {
		int8_t x2 = relX + sideVectors[side][0];
		int8_t y2 = y + sideVectors[side][1];
		int8_t z2 = relZ + sideVectors[side][2];
		
		bool renderFace;
		if(INVALID_BLOCK_POS(x2, y2, z2)) {
			int32_t x3 = chunkX*CHUNK_SIZE + x2;
			int32_t z3 = chunkZ*CHUNK_SIZE + z2;
			renderFace = !world->hasBlock(x3, y2, z3)
				|| (!world->isOpaqueCube(x3, y2, z3) && block != world->getBlock(x3, y2, z3));
		} else {
			renderFace = !chunk.hasBlock(x2, y2, z2)
				|| (!chunk.isOpaqueCube(x2, y2, z2) && block != chunk.getBlock(x2, y2, z2));
		}
		if(renderFace) {
			FaceData face = {
				(float) relX, (float) y, (float) relZ, side, block->getFaceTexture(side)
			};
			if(block->rendering() == BlockRendering::translucentCube) {
				translucentBuffer.faces.push_back(face);
			} else {
				buffer.faces.push_back(face);
			}
		}
	}
}


ChunkRenderer::ChunkRenderer(FaceRenderer& renderer, int renderDist)
	: faceRenderer(renderer), renderDist(renderDist) { }

void ChunkRenderer::prerenderChunk(World& world, int32_t chunkX, int32_t chunkZ) {
	uint64_t key = packCoords(chunkX, chunkZ);
	RenderedChunk& renderedChunk = renderedChunks[key];
	if(!renderedChunk.isInitialized())
		renderedChunk.init(faceRenderer, world, chunkX, chunkZ);
	renderedChunk.prerender();
	
	// Update nearby chunks
	auto chunkIter = renderedChunks.find(packCoords(chunkX - 1, chunkZ));
	if(chunkIter != renderedChunks.end())
		chunkIter->second.updatePlaneX(CHUNK_SIZE - 1);
	chunkIter = renderedChunks.find(packCoords(chunkX + 1, chunkZ));
	if(chunkIter != renderedChunks.end())
		chunkIter->second.updatePlaneX(0);
	chunkIter = renderedChunks.find(packCoords(chunkX, chunkZ - 1));
	if(chunkIter != renderedChunks.end())
		chunkIter->second.updatePlaneZ(CHUNK_SIZE - 1);
	chunkIter = renderedChunks.find(packCoords(chunkX, chunkZ + 1));
	if(chunkIter != renderedChunks.end())
		chunkIter->second.updatePlaneZ(0);
}

void ChunkRenderer::updateBlock(World& world, int32_t x, int8_t y, int32_t z) {
	updateSingleBlock(world, x, y, z);
	
	for(uint8_t side = 0; side < 6; ++side) {
		int8_t x2 = x + sideVectors[side][0];
		int8_t y2 = y + sideVectors[side][1];
		int8_t z2 = z + sideVectors[side][2];
		
		if(y2 >= 0 && y2 < CHUNK_HEIGHT)
			updateSingleBlock(world, x2, y2, z2);
	}
}

void ChunkRenderer::render(int32_t camChunkX, int32_t camChunkZ) {
	for(int32_t x = camChunkX - renderDist; x <= camChunkX + renderDist; ++x) {
		for(int32_t z = camChunkZ - renderDist; z <= camChunkZ + renderDist; ++z) {
			auto chunkIter = renderedChunks.find(packCoords(x, z));
			if(chunkIter != renderedChunks.end()) {
				chunkIter->second.render(faceRenderer);
			}
		}
	}
	
	glDepthMask(GL_FALSE);
	for(int32_t x = camChunkX - renderDist; x <= camChunkX + renderDist; ++x) {
		for(int32_t z = camChunkZ - renderDist; z <= camChunkZ + renderDist; ++z) {
			auto chunkIter = renderedChunks.find(packCoords(x, z));
			if(chunkIter != renderedChunks.end()) {
				chunkIter->second.renderTranslucent(faceRenderer);
			}
		}
	}
	glDepthMask(GL_TRUE);
}

void ChunkRenderer::updateSingleBlock(World& world, int32_t x, int8_t y, int32_t z) {
	int32_t chunkX, chunkZ;
	std::tie(chunkX, chunkZ) = World::getChunkAt(x, z);
	int8_t relX = x - chunkX*CHUNK_SIZE;
	int8_t relZ = z - chunkZ*CHUNK_SIZE;
	uint64_t key = packCoords(chunkX, chunkZ);
	auto chunkIter = renderedChunks.find(key);
	if(chunkIter != renderedChunks.end()) {
		chunkIter->second.updateBlock(relX, y, relZ);
	}
}