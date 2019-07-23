#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include <iostream>

#include "glm.hpp"
#include "glfw.hpp"
#define GLT_MANUAL_VIEWPORT
#include <gltext.h>

class TextRenderer;

class Text {
	friend TextRenderer;
	
public:
	Text(std::string str, float x, float y, float scale, glm::vec4 color);
	void deleteText();
	
	void setText(std::string str);
	
private:
	GLTtext* text;
	float x, y, scale;
	glm::vec4 color;
	
	void render();
};

class TextRenderer {
public:
	void init();
	void terminate();
	
	void setViewport(int width, int height);
	
	Text& createText(std::string str, float x, float y, float scale, glm::vec4 color);
	
	void render();
	
private:
	std::vector<Text> texts;
};
