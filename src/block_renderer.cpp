#include "block_renderer.hpp"

const float faceVertices[] = {
	 0.5f, -0.5f,  0.5f,  1.0, 0.0,
	 0.5f,  0.5f,  0.5f,  1.0, 1.0,
	-0.5f, -0.5f,  0.5f,  0.0, 0.0,
	-0.5f,  0.5f,  0.5f,  0.0, 1.0
};

const glm::mat4 id = glm::mat4(1.0f);
const glm::mat4 sideTransforms[6] = {
	glm::mat4(1.0f),
	glm::rotate(id, TAU/4, glm::vec3(0.0f, 1.0f, 0.0f)),
	glm::rotate(id, TAU/2, glm::vec3(0.0f, 1.0f, 0.0f)),
	glm::rotate(id, -TAU/4, glm::vec3(0.0f, 1.0f, 0.0f)),
	glm::rotate(id, TAU/4, glm::vec3(1.0f, 0.0f, 0.0f)),
	glm::rotate(id, -TAU/4, glm::vec3(1.0f, 0.0f, 0.0f))
};
const int sideVectors[6][3] = {
	{0,0,1}, {1,0,0}, {0,0,-1}, {-1,0,0}, {0,-1,0}, {0,1,0}
};

const size_t BLOCK_TEX_SIZE = 16;
const size_t BLOCK_TEX_COUNT = 5;
const char* textureFiles[BLOCK_TEX_COUNT] = {
	"placeholder", "stone", "dirt", "grass_side", "grass_top"
};


RenderedChunk::RenderedChunk() : faceCount(0), VBO(0), VAO(0) { }

void RenderedChunk::init(uint32_t faceVBO) {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, faceVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3*sizeof(float)));
	glEnableVertexAttribArray(1);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	size_t faceDataSize = sizeof(FaceData);
	glBufferData(GL_ARRAY_BUFFER, faceDataSize*MAX_CHUNK_FACES, nullptr, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, faceDataSize, (void*) offsetof(FaceData, offsetX));
	glVertexAttribDivisor(2, 1);
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(3, 1, GL_UNSIGNED_BYTE, faceDataSize, (void*) offsetof(FaceData, side));
	glVertexAttribDivisor(3, 1);
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, faceDataSize, (void*) offsetof(FaceData, texId));
	glVertexAttribDivisor(4, 1);
	glEnableVertexAttribArray(4);
	
	glBindVertexArray(0);
}

bool RenderedChunk::isInitialized() { return VBO != 0; }

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
	
	faceCount = faces.size();
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, faceCount * sizeof(FaceData), faces.data());
}

void RenderedChunk::render() {
	glBindVertexArray(VAO);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, faceCount);
	glBindVertexArray(0);
}


BlockRenderer::BlockRenderer(int renderDist)
	: renderDist(renderDist), fogEnd(renderDist * CHUNK_SIZE), fogStart(renderDist * CHUNK_SIZE * 0.9f) { }

void BlockRenderer::init() {
	program = loadShaders();
	
	glGenBuffers(1, &faceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, faceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(faceVertices), faceVertices, GL_STATIC_DRAW);
	
	glGenTextures(1, &textureArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, BLOCK_TEX_SIZE, BLOCK_TEX_SIZE, BLOCK_TEX_COUNT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	for(size_t i = 0; i < BLOCK_TEX_COUNT; ++i) {
		std::string filename = "res/" + std::string(textureFiles[i]) + ".png";
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
		if(!data) throw std::runtime_error("Failed to load block texture");
		if(width != BLOCK_TEX_SIZE || height != BLOCK_TEX_SIZE) throw std::runtime_error("Block texture has incorrect dimensions");
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, BLOCK_TEX_SIZE, BLOCK_TEX_SIZE, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}

void BlockRenderer::renderChunk(Chunk& chunk, int32_t x, int32_t z) {
	uint64_t key = getChunkId(x, z);
	RenderedChunk& renderedChunk = renderedChunks[key];
	if(!renderedChunk.isInitialized())
		renderedChunk.init(faceVBO);
	renderedChunk.load(chunk);
}

void BlockRenderer::render(glm::mat4 proj, glm::mat4 view, int32_t camChunkX, int32_t camChunkZ, const float skyColor[3]) {
	glUseProgram(program);
	
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(glGetUniformLocation(program, "sideTransforms"), 6, GL_FALSE, glm::value_ptr(sideTransforms[0]));
	
	glUniform1i(glGetUniformLocation(program, "tex"), 0);
	
	glUniform1f(glGetUniformLocation(program, "ambientLight"), 0.7);
	glUniform1f(glGetUniformLocation(program, "diffuseLight"), 0.3);
	glm::vec3 lightSrcDir = glm::normalize(glm::vec3(0.5f, 1.0f, 0.1f));
	glUniform3fv(glGetUniformLocation(program, "lightSrcDir"), 1, glm::value_ptr(lightSrcDir));
	
	glUniform4f(glGetUniformLocation(program, "fogColor"), skyColor[0], skyColor[1], skyColor[2], 1.0f);
	glUniform1f(glGetUniformLocation(program, "fogStart"), fogStart);
	glUniform1f(glGetUniformLocation(program, "fogEnd"), fogEnd);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
	
	for(int32_t x = camChunkX - renderDist; x <= camChunkX + renderDist; ++x) {
		for(int32_t z = camChunkZ - renderDist; z <= camChunkZ + renderDist; ++z) {
			uint64_t key = getChunkId(x, z);
			auto chunkIter = renderedChunks.find(key);
			if(chunkIter != renderedChunks.end()) {
				glm::mat4 model = glm::translate(id, ((float) CHUNK_SIZE) * glm::vec3(x, 0.0f, z));
				glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
				chunkIter->second.render();
			}
		}
	}
}
