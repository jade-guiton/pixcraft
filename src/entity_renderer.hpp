#pragma once

#include <cstddef>

#include "glfw.hpp"
#include "glm.hpp"

#include "textures.hpp"

class EntityModel {
public:
	EntityModel();
	void init(TexId texture, float* vertices, size_t vertexCount, unsigned int* indices, size_t indexCount);
	
	void bindTexture();
	void render();
	
private:
	GlId VAO;
	TexId texture;
	size_t indexCount;
};

class EntityRenderer {
public:
	EntityRenderer();
	void init();
	
	void startRendering(glm::mat4 proj, glm::mat4 view, RenderParams params);
	void render(EntityModel& model, glm::mat4 modelMat);
	
private:
	GlId program;
};
