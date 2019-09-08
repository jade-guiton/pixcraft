#include "face_renderer.hpp"

#include <stdexcept>
#include <string>
#include <cstddef>

#include "pixcraft/util/util.hpp"

using namespace PixCraft;

const std::array<glm::mat3, 6> sideTransforms = {
	glm::mat3(1.0f),
	glm::mat3(glm::rotate(glm::mat4(1.0f), TAU/4, glm::vec3(0.0f, 1.0f, 0.0f))),
	glm::mat3(glm::rotate(glm::mat4(1.0f), TAU/2, glm::vec3(0.0f, 1.0f, 0.0f))),
	glm::mat3(glm::rotate(glm::mat4(1.0f), -TAU/4, glm::vec3(0.0f, 1.0f, 0.0f))),
	glm::mat3(glm::rotate(glm::mat4(1.0f), TAU/4, glm::vec3(1.0f, 0.0f, 0.0f))),
	glm::mat3(glm::rotate(glm::mat4(1.0f), -TAU/4, glm::vec3(1.0f, 0.0f, 0.0f)))
};


void FaceBuffer::init(FaceRenderer& faceRenderer, int capacity) {
	buffer.init(offsetof(FaceData, offsetX), offsetof(FaceData, side),
		offsetof(FaceData, texId), sizeof(FaceData));
	buffer.loadData(nullptr, capacity, GL_STATIC_DRAW);
	faces.reserve(capacity);
	checkGlErrors("face buffer initialization");
}

bool FaceBuffer::isInitialized() {
	return buffer.isInitialized();
}

void FaceBuffer::prerender() {
	size_t faceCount = faces.size();
	if(faceCount > buffer.vertexCount()) throw std::logic_error("Too many faces loaded into FaceBuffer");
	buffer.updateData(faces.data(), faceCount);
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
	buffer.bind();
	glDrawArrays(GL_POINTS, 0, faces.size());
	buffer.unbind();
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
