#pragma once

#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <cmath>
#include <tuple>
#include <string>

#include "glfw.hpp"
#include "glm.hpp"

#include "shaders.hpp"
#include "block_renderer.hpp"
#include "blocks.hpp"
#include "util.hpp"
#include "input.hpp"
#include "text.hpp"
#include "player.hpp"


class GameClient {
public:
	GameClient();
	~GameClient();
	
	void mainLoop();
	
	InputManager& getInputManager();
	TextRenderer& getTextRenderer();
	
private:
	static const int START_WIDTH = 800;
	static const int START_HEIGHT = 600;
	static constexpr float SKY_COLOR[3] = {0.75f, 0.9f, 1.0f};
	
	static const int RENDER_DIST = 6;
	static const int LOADS_PER_FRAME = 2;
	
	GLFWwindow* window;
	
	InputManager input;
	World world;
	Player* player;
	BlockRenderer blockRenderer;
	
	TextRenderer textRenderer;
	Text* debugText;
	
	GlId cursorProgram;
	GlId cursorVAO;
	
	bool paused;
	bool firstFrame;
	int FPS;
	
	std::tuple<bool, int, int, int> blockRaycast(bool offset);
	
	void update(float dt);
	void render();
	
	friend void windowResizeCallback(GLFWwindow* window, int width, int height);
};
