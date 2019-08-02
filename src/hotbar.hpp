#pragma once

#include "glfw.hpp"
#include "glm.hpp"

#include "blocks.hpp"
#include "shaders.hpp"

class Hotbar {
public:
	Hotbar();
	void init();
	
	void setBlock(BlockId blockId);
	
	void render(glm::mat4 proj);
	
private:
	GlId program, VBO, textureArray;
};
