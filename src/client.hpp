#pragma once

#include <tuple>

#include "glfw.hpp"

#include "face_renderer.hpp"
#include "chunk_renderer.hpp"
#include "hotbar.hpp"
#include "input.hpp"
#include "text.hpp"
#include "player.hpp"
#include "world.hpp"


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
	static constexpr float FOG_END = RENDER_DIST * CHUNK_SIZE;
	static constexpr float FOG_START = FOG_END * 0.9f;
	static const int LOADS_PER_FRAME = 2;
	
	GLFWwindow* window;
	
	InputManager input;
	World world;
	Player* player;
	
	FaceRenderer faceRenderer;
	ChunkRenderer chunkRenderer;
	Hotbar hotbar;
	
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
