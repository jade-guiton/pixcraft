#include "entity_renderer.hpp"

#include <cstddef>

#include "shaders.hpp"

EntityModel::EntityModel() : VAO(0) {}

void EntityModel::init(TexId texture2, float* vertices, size_t vertexCount, unsigned int* indices, size_t indexCount2) {
	texture = texture2;
	indexCount = indexCount2;
	
	glGenVertexArrays(1, &VAO);
	GlId VBO, EBO;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	size_t floatSize = sizeof(float);
	size_t vertexSize = floatSize*5;
	glBufferData(GL_ARRAY_BUFFER, vertexSize*vertexCount, vertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*) 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexSize, (void*) (floatSize*3));
	glEnableVertexAttribArray(1);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount, indices, GL_STATIC_DRAW);
	
	glBindVertexArray(0);
}

void EntityModel::bindTexture() {
	TextureManager::bindOtherTextures(texture);
}

void EntityModel::render() {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


EntityRenderer::EntityRenderer() {}

void EntityRenderer::init() {
	program = loadEntityProgram();
}

void EntityRenderer::startRendering(glm::mat4 proj, glm::mat4 view, RenderParams params) {
	glUseProgram(program);
	
	glUniform1i(glGetUniformLocation(program, "applyView"), params.applyView);
	glUniform1i(glGetUniformLocation(program, "applyFog"), params.applyFog);
	glUniform4f(glGetUniformLocation(program, "fogColor"), params.skyColor[0], params.skyColor[1], params.skyColor[2], 1.0f);
	glUniform1f(glGetUniformLocation(program, "fogStart"), params.fogStart);
	glUniform1f(glGetUniformLocation(program, "fogEnd"), params.fogEnd);
	
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
	
	glUniform1i(glGetUniformLocation(program, "tex"), 0);
	
	glUniform1f(glGetUniformLocation(program, "ambientLight"), 0.7);
}

void EntityRenderer::render(EntityModel& model, glm::mat4 modelMat) {
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(modelMat));
	
	model.render();
}
