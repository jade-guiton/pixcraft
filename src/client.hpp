#pragma once

#include <iostream>
#include <stdexcept>
#include <cstdint>

#include "glfw.hpp"
#include "glm.hpp"

#include "block_renderer.hpp"
#include "blocks.hpp"

#define WINDOW_START_WIDTH 800
#define WINDOW_START_HEIGHT 600

class GameClient {
public:
	GameClient();
	~GameClient();
	
	void mainLoop();
	
private:
	GLFWwindow* window;
	int width, height;
	
	BlockRenderer blockRenderer;
	
	glm::vec3 camPos, camOrient;
	double oldMouseX, oldMouseY;
	
	bool firstFrame;
	
	void update();
	void render();
	
	friend void windowResizeCallback(GLFWwindow* window, int width, int height);
};
