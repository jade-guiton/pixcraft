#include "entity_renderer.hpp"

#include <cstddef>
#include <typeinfo>
#include <array>

#include "pixcraft/server/world.hpp"
#include "pixcraft/server/mob.hpp"
#include "pixcraft/server/slime.hpp"

using namespace PixCraft;

const std::array<float, 70> slimeVertices = {
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

const std::array<unsigned int, 36> slimeIndices = {
	0,  1,  3,    3,  1,  4, // bottom
	2,  3,  7,    7,  3,  8, // left
	3,  4,  8,    8,  4,  9, // front
	4,  5,  9,    9,  5,  10, // right
	5,  6,  10,   10, 6,  11, // back
	8,  9,  12,   12, 9,  13
};

template<size_t vertexCount, size_t indexCount>
void EntityModel::init(TexId texture2, const std::array<float, vertexCount>& vertices,
		const std::array<unsigned int, indexCount>& indices, glm::mat4 preModel) {
	texture = texture2;
	_preModel = preModel;
	
	buffer.init(0, 3*sizeof(float), 5*sizeof(float));
	buffer.loadData(vertices.data(), vertexCount / 5, GL_STATIC_DRAW);
	buffer.loadIndices(indices.data(), indexCount);
	checkGlErrors("entity model initialization");
}

glm::mat4 EntityModel::preModel() { return _preModel; }

void EntityModel::bindTexture() {
	TextureManager::bindOtherTextures(texture);
}

void EntityModel::render() {
	buffer.bind();
	glDrawElements(GL_TRIANGLES, buffer.indexCount(), GL_UNSIGNED_INT, 0);
	buffer.unbind();
}


EntityRenderer::EntityRenderer() {}

void EntityRenderer::init() {
	program.init(ShaderSources::entityVS, ShaderSources::entityFS);
	
	glm::mat4 preModel = glm::translate(glm::scale(glm::mat4(1.0), glm::vec3(0.3f, 0.3f, 0.3f)), glm::vec3(0.0f, 1.0f, 0.0f));
	slimeModel.init(TEX(SLIME), slimeVertices, slimeIndices, preModel);
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
	program.use();
	
	program.setUniform("applyView", params.applyView);
	program.setUniform("applyFog", params.applyFog);
	program.setUniform("fogColor", params.skyColor[0], params.skyColor[1], params.skyColor[2], 1.0f);
	program.setUniform("fogStart", params.fogStart);
	program.setUniform("fogEnd", params.fogEnd);
	
	program.setUniform("view", view);
	program.setUniform("proj", proj);
	
	program.setUniform("tex", (uint32_t) 0);
	
	program.setUniform("ambientLight", 0.7f);
}

void EntityRenderer::render(EntityModel& model, glm::mat4 modelMat) {
	glm::mat4 preModel = model.preModel();
	modelMat = modelMat * preModel;
	program.setUniform("model", modelMat);
	
	model.render();
}

void EntityRenderer::stopRendering() {
	program.unuse();
}
