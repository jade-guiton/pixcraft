#pragma once

#include <cstdint>
#include <unordered_map>

#include "glfw.hpp"
#include "glm.hpp"


struct Character {
	GlId texId;
	glm::ivec2 size;
	glm::ivec2 bearing;
	int32_t advance;
};


class TextRenderer2 {
public:
	void init();
	
	void setViewport(int width, int height);
	
	void render();
	
private:
	GlId program, VAO, VBO;
	int width, height;
	std::unordered_map<char, Character> characters;
};
