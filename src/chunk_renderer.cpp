#include "chunk_renderer.hpp"

#include <stdexcept>
#include <string>
#include <vector>

#include "glm.hpp"

#include "blocks.hpp"
#include "util.hpp"

void RenderedChunk::init(FaceRenderer& faceRenderer) {
	buffer.init(faceRenderer, MAX_CHUNK_FACES);
}

bool RenderedChunk::isInitialized() { return buffer.isInitialized(); }

void RenderedChunk::load(Chunk& chunk) {
	std::vector<FaceData> faces;
	for(uint8_t x = 0; x < CHUNK_SIZE; ++x) {
		for(uint8_t y = 0; y < CHUNK_HEIGHT; ++y) {
			for(uint8_t z = 0; z < CHUNK_SIZE; ++z) {
				Block* block = chunk.getBlock(x, y, z);
				if(block == nullptr) continue;
				
				if(block->isOpaqueCube()) {
					for(uint8_t side = 0; side < 6; ++side) {
						int8_t x2 = x + sideVectors[side][0];
						int8_t y2 = y + sideVectors[side][1];
						int8_t z2 = z + sideVectors[side][2];
						if(INVALID_BLOCK_POS(x2, y2, z2) || !chunk.isOpaqueCube(x2, y2, z2)) {
							faces.push_back(FaceData {
								(float) x, (float) y, (float) z, side, block->getFaceTexture(side)
							});
						}
					}
				}
			}
		}
	}
	
	buffer.load(faces);
}

void RenderedChunk::render(FaceRenderer& faceRenderer, int32_t x, int32_t z) {
	glm::mat4 model = glm::translate(glm::mat4(1.0f), ((float) CHUNK_SIZE) * glm::vec3(x, 0.0f, z));
	faceRenderer.render(buffer, model);
}


ChunkRenderer::ChunkRenderer(FaceRenderer& renderer, int renderDist)
	: faceRenderer(renderer), renderDist(renderDist) { }

void ChunkRenderer::prerenderChunk(Chunk& chunk, int32_t x, int32_t z) {
	uint64_t key = packCoords(x, z);
	RenderedChunk& renderedChunk = renderedChunks[key];
	if(!renderedChunk.isInitialized())
		renderedChunk.init(faceRenderer);
	renderedChunk.load(chunk);
}

void ChunkRenderer::render(int32_t camChunkX, int32_t camChunkZ) {
	for(int32_t x = camChunkX - renderDist; x <= camChunkX + renderDist; ++x) {
		for(int32_t z = camChunkZ - renderDist; z <= camChunkZ + renderDist; ++z) {
			uint64_t key = packCoords(x, z);
			auto chunkIter = renderedChunks.find(key);
			if(chunkIter != renderedChunks.end()) {
				chunkIter->second.render(faceRenderer, x, z);
			}
		}
	}
}
