#pragma once

#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <cmath>

#include "glfw.hpp"
#include "glm.hpp"

#include "block_renderer.hpp"
#include "blocks.hpp"
#include "util.hpp"
#include "input.hpp"


class GameClient {
public:
	GameClient();
	~GameClient();
	
	void mainLoop();
	
private:
	static const int START_WIDTH = 800;
	static const int START_HEIGHT = 600;
	static constexpr float SKY_COLOR[3] = {0.75f, 0.9f, 1.0f};
	
	static const int RENDER_DIST = 7;
	static const int LOADS_PER_FRAME = 2;
	
	static constexpr float MVT_SPEED = 50.0f;
	
	GLFWwindow* window;
	
	InputManager input;
	World world;
	BlockRenderer blockRenderer;
	
	bool paused;
	bool firstFrame;
	int FPS;
	
	void update(float dt);
	void render();
	
	friend void windowResizeCallback(GLFWwindow* window, int width, int height);
};
