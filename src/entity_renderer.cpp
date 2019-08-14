#include "entity_renderer.hpp"

#include <cstddef>
#include <typeinfo>

#include "shaders.hpp"

#include "world/world.hpp"
#include "world/mob.hpp"
#include "world/slime.hpp"

const float slimeVertices[70] = {
	 1, -1,  1,  1.0/4, 0.0/3,
	-1, -1,  1,  2.0/4, 0.0/3,
	 1, -1,  1,  0.0/4, 1.0/3,
	 1, -1, -1,  1.0/4, 1.0/3,
	-1, -1, -1,  2.0/4, 1.0/3,
	-1, -1,  1,  3.0/4, 1.0/3,
	 1, -1,  1,  4.0/4, 1.0/3,
	 1,  1,  1,  0.0/4, 2.0/3,
	 1,  1, -1,  1.0/4, 2.0/3,
	-1,  1, -1,  2.0/4, 2.0/3,
	-1,  1,  1,  3.0/4, 2.0/3,
	 1,  1,  1,  4.0/4, 2.0/3,
	 1,  1,  1,  1.0/4, 3.0/3,
	-1,  1,  1,  2.0/4, 3.0/3
};

const unsigned int slimeIndices[36] = {
	0,  1,  3,    3,  1,  4, // bottom
	2,  3,  7,    7,  3,  8, // left
	3,  4,  8,    8,  4,  9, // front
	4,  5,  9,    9,  5,  10, // right
	5,  6,  10,   10, 6,  11, // back
	8,  9,  12,   12, 9,  13
};

EntityModel::EntityModel() : VAO(0) {}

void EntityModel::init(TexId texture2, const float* vertices, size_t vertexCount, const unsigned int* indices,
		size_t indexCount2, glm::mat4 preModel) {
	texture = texture2;
	indexCount = indexCount2;
	_preModel = preModel;
	
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount*sizeof(unsigned int), indices, GL_STATIC_DRAW);
	
	glBindVertexArray(0);
	checkGlErrors("entity renderer initialization");
}

glm::mat4 EntityModel::preModel() { return _preModel; }

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
	
	glm::mat4 preModel = glm::translate(glm::scale(glm::mat4(1.0), glm::vec3(0.3f, 0.3f, 0.3f)), glm::vec3(0.0f, 1.0f, 0.0f));
	slimeModel.init(TEX(SLIME), slimeVertices, 14, slimeIndices, 36, preModel);
}

void EntityRenderer::renderEntities(World& world, glm::mat4 proj, glm::mat4 view, RenderParams params) {
	startRendering(proj, view, params);
	
	EntityModel* model;
	for(auto it = world.mobs.begin(); it != world.mobs.end(); ++it) {
		model = nullptr;
		if(typeid(**it) == typeid(Slime)) {
			model = &slimeModel;
		}
		if(model != nullptr) {
			glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), (*it)->pos());
			model->bindTexture();
			render(*model, modelMat);
		}
	}
	
	stopRendering();
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
	glm::mat4 preModel = model.preModel();
	modelMat = modelMat * preModel;
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(modelMat));
	
	model.render();
}

void EntityRenderer::stopRendering() {
	glUseProgram(0);
}
