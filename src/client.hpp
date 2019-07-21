#pragma once

#include <iostream>
#include <stdexcept>
#include <cstdint>

#include "glfw.hpp"
#include "glm.hpp"

#include "block_renderer.hpp"
#include "blocks.hpp"


class GameClient {
public:
	GameClient();
	~GameClient();
	
	void mainLoop();
	
private:
	static const int START_WIDTH = 800;
	static const int START_HEIGHT = 600;
	static constexpr float SKY_COLOR[3] = {0.75f, 0.9f, 1.0f};
	
	GLFWwindow* window;
	int width, height;
	
	World world;
	BlockRenderer blockRenderer;
	
	double oldMouseX, oldMouseY;
	bool firstFrame;
	double lastSecond;
	int frameCounter, FPS;
	
	void update();
	void render();
	
	friend void windowResizeCallback(GLFWwindow* window, int width, int height);
};
