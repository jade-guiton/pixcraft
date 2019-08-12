#include "face_renderer.hpp"

#include <stdexcept>
#include <string>
#include <cstddef>

#include "shaders.hpp"
#include "textures.hpp"
#include "util.hpp"

const float faceVertices[] = {
	 0.5f, -0.5f,  0.5f,  1.0, 0.0,
	 0.5f,  0.5f,  0.5f,  1.0, 1.0,
	-0.5f, -0.5f,  0.5f,  0.0, 0.0,
	-0.5f,  0.5f,  0.5f,  0.0, 1.0
};

const glm::mat3 sideTransforms[6] = {
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
	
	faceRenderer.bindFaceAttributes();
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	size_t faceDataSize = sizeof(FaceData);
	glBufferData(GL_ARRAY_BUFFER, faceDataSize*newCapacity, nullptr, GL_STATIC_DRAW);
	faces.reserve(newCapacity);
	capacity = newCapacity;
	
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
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, faces.size());
	glBindVertexArray(0);
}


FaceRenderer::FaceRenderer() { }

void FaceRenderer::init() {
	program = loadBlockProgram();
	
	glGenBuffers(1, &faceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, faceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(faceVertices), faceVertices, GL_STATIC_DRAW);
}

void FaceRenderer::bindFaceAttributes() {
	glBindBuffer(GL_ARRAY_BUFFER, faceVBO);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3*sizeof(float)));
	glEnableVertexAttribArray(1);
}

void FaceRenderer::setParams(RenderParams params) {
	glUniform1i(glGetUniformLocation(program, "applyView"), params.applyView);
	glUniform1i(glGetUniformLocation(program, "applyFog"), params.applyFog);
	glUniform4f(glGetUniformLocation(program, "fogColor"), params.skyColor[0], params.skyColor[1], params.skyColor[2], 1.0f);
	glUniform1f(glGetUniformLocation(program, "fogStart"), params.fogStart);
	glUniform1f(glGetUniformLocation(program, "fogEnd"), params.fogEnd);
}

void FaceRenderer::startRendering(glm::mat4 proj, glm::mat4 view, RenderParams params) {
	glUseProgram(program);
	
	setParams(params);
	
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix3fv(glGetUniformLocation(program, "sideTransforms"), 6, GL_FALSE, glm::value_ptr(sideTransforms[0]));
	
	glUniform1i(glGetUniformLocation(program, "tex"), 0);
	
	glUniform1f(glGetUniformLocation(program, "ambientLight"), 0.7);
	glUniform1f(glGetUniformLocation(program, "diffuseLight"), 0.3);
	glm::vec3 lightSrcDir = glm::normalize(glm::vec3(0.5f, 1.0f, 0.1f));
	glUniform3fv(glGetUniformLocation(program, "lightSrcDir"), 1, glm::value_ptr(lightSrcDir));
	
	TextureManager::bindBlockTextureArray();
}

void FaceRenderer::render(FaceBuffer& buffer, glm::mat4 model) {
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	
	buffer.render();
}

void FaceRenderer::stopRendering() {
	glUseProgram(0);
}
