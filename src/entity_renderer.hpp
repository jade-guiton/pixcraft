#pragma once

#include <cstddef>

#include "glfw.hpp"
#include "glm.hpp"

#include "shaders.hpp"
#include "textures.hpp"

#include "world/world_module.hpp"

class EntityModel {
public:
	EntityModel();
	void init(TexId texture, const float* vertices, size_t vertexCount, const unsigned int* indices,
		size_t indexCount, glm::mat4 preModel);
	
	glm::mat4 preModel();
	
	void bindTexture();
	void render();
	
private:
	GlId VAO;
	TexId texture;
	size_t indexCount;
	glm::mat4 _preModel;
};

class EntityRenderer {
public:
	EntityRenderer();
	void init();
	
	void renderEntities(World& world, glm::mat4 proj, glm::mat4 view, RenderParams params);
	
private:
	ShaderProgram program;
	
	EntityModel slimeModel;
	
	void startRendering(glm::mat4 proj, glm::mat4 view, RenderParams params);
	void render(EntityModel& model, glm::mat4 modelMat);
	void stopRendering();
};
