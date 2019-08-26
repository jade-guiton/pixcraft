#include "face_renderer.hpp"

#include <stdexcept>
#include <string>
#include <cstddef>

#include "util.hpp"


const std::array<glm::mat3, 6> sideTransforms = {
	glm::mat3(1.0f),
	glm::mat3(glm::rotate(glm::mat4(1.0f), TAU/4, glm::vec3(0.0f, 1.0f, 0.0f))),
	glm::mat3(glm::rotate(glm::mat4(1.0f), TAU/2, glm::vec3(0.0f, 1.0f, 0.0f))),
	glm::mat3(glm::rotate(glm::mat4(1.0f), -TAU/4, glm::vec3(0.0f, 1.0f, 0.0f))),
	glm::mat3(glm::rotate(glm::mat4(1.0f), TAU/4, glm::vec3(1.0f, 0.0f, 0.0f))),
	glm::mat3(glm::rotate(glm::mat4(1.0f), -TAU/4, glm::vec3(1.0f, 0.0f, 0.0f)))
};


FaceBuffer::FaceBuffer()
	: VAO(0), VBO(0), capacity(0) { }

FaceBuffer::~FaceBuffer() {
	if(VAO != 0) {
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
	}
}

void FaceBuffer::init(FaceRenderer& faceRenderer, int newCapacity) {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	size_t faceDataSize = sizeof(FaceData);
	glBufferData(GL_ARRAY_BUFFER, faceDataSize*newCapacity, nullptr, GL_STATIC_DRAW);
	faces.reserve(newCapacity);
	capacity = newCapacity;
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, faceDataSize, (void*) offsetof(FaceData, offsetX));
	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(1, 1, GL_UNSIGNED_BYTE, faceDataSize, (void*) offsetof(FaceData, side));
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, faceDataSize, (void*) offsetof(FaceData, texId));
	glEnableVertexAttribArray(2);
	
	glBindVertexArray(0);
	checkGlErrors("face buffer initialization");
}

bool FaceBuffer::isInitialized() { return VAO != 0; }

void FaceBuffer::prerender() {
	size_t faceCount = faces.size();
	if(faceCount > capacity) throw std::logic_error("Too many faces loaded into FaceBuffer");
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, faceCount * sizeof(FaceData), faces.data());
}

void FaceBuffer::eraseFaces(int8_t x, int8_t y, int8_t z) {
	size_t i = 0;
	while(i < faces.size()) {
		FaceData& face = faces[i];
		if(face.offsetX == (float) x && face.offsetY == (float) y && face.offsetZ == (float) z) {
			faces.erase(faces.begin() + i);
		} else {
			i++;
		}
	}
}

void FaceBuffer::erasePlaneX(int8_t x) {
	size_t i = 0;
	while(i < faces.size()) {
		FaceData& face = faces[i];
		if(face.offsetX == (float) x) {
			faces.erase(faces.begin() + i);
		} else {
			i++;
		}
	}
}

void FaceBuffer::erasePlaneZ(int8_t z) {
	size_t i = 0;
	while(i < faces.size()) {
		FaceData& face = faces[i];
		if(face.offsetZ == (float) z) {
			faces.erase(faces.begin() + i);
		} else {
			i++;
		}
	}
}

void FaceBuffer::render() {
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, faces.size());
	glBindVertexArray(0);
}


FaceRenderer::FaceRenderer() { }

void FaceRenderer::init() {
	program.init(ShaderSources::blockVS, ShaderSources::blockGS, ShaderSources::blockFS);
	checkGlErrors("face renderer initialization");
}

void FaceRenderer::setParams(RenderParams params) {
	program.setUniform("applyView", params.applyView);
	program.setUniform("applyFog", params.applyFog);
	program.setUniform("fogColor", params.skyColor[0], params.skyColor[1], params.skyColor[2], 1.0f);
	program.setUniform("fogStart", params.fogStart);
	program.setUniform("fogEnd", params.fogEnd);
}

void FaceRenderer::startRendering(glm::mat4 proj, glm::mat4 view, RenderParams params) {
	program.use();
	
	setParams(params);
	
	program.setUniform("view", view);
	program.setUniform("proj", proj);
	program.setUniformArray("sideTransforms", sideTransforms);
	
	program.setUniform("texArray", (uint32_t) 0);
	
	program.setUniform("ambientLight", 0.7f);
	program.setUniform("diffuseLight", 0.3f);
	glm::vec3 lightSrcDir = glm::normalize(glm::vec3(0.5f, 1.0f, 0.1f));
	program.setUniform("lightSrcDir", lightSrcDir);
	
	TextureManager::bindBlockTextureArray();
}

void FaceRenderer::render(FaceBuffer& buffer, glm::mat4 model) {
	program.setUniform("model", model);
	
	buffer.render();
}

void FaceRenderer::stopRendering() {
	program.unuse();
}
