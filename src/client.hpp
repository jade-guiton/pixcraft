#pragma once

#include <tuple>

#include "glfw.hpp"
#include "glm.hpp"

#include "face_renderer.hpp"
#include "chunk_renderer.hpp"
#include "entity_renderer.hpp"
#include "hotbar.hpp"
#include "input.hpp"
#include "text.hpp"

#include "world/world_module.hpp"
#include "world/world.hpp"
#include "world/chunk.hpp"

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
	
	static const int LOADS_PER_FRAME = 1;
	
	static constexpr float PLAYER_REACH = 3.0f;
	
	GLFWwindow* window;
	
	bool antialiasing;
	bool showDebug;
	bool paused;
	int renderDist;
	float fogStart, fogEnd;
	
	bool showCommandLine;
	std::string commandBuffer;
	
	InputManager input;
	World world;
	Player* player;
	
	FaceRenderer faceRenderer;
	ChunkRenderer chunkRenderer;
	EntityRenderer entityRenderer;
	Hotbar hotbar;
	TextRenderer textRenderer;
	
	ShaderProgram cursorProgram;
	IndexBuffer<glm::vec2> cursorBuffer;
	ShaderProgram colorOverlayProgram;
	VertexBuffer<glm::vec2> colorOverlayBuffer;
	ShaderProgram blockOverlayProgram;
	IndexBuffer<glm::vec3> blockOverlayBuffer;
	
	int frameNo;
	int FPS;
	
	void setAntialiasing(bool enabled);
	void setRenderDistance(int renderDist);
	
	void executeCommand();
	void update(float dt);
	void render();
	
	friend void windowResizeCallback(GLFWwindow* window, int width, int height);
};
