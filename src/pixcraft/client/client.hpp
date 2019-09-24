#pragma once

#include <tuple>

#include "glfw.hpp"
#include "pixcraft/util/glm.hpp"

#include "face_renderer.hpp"
#include "chunk_renderer.hpp"
#include "entity_renderer.hpp"
#include "particle_renderer.hpp"
#include "hotbar.hpp"
#include "input.hpp"
#include "text.hpp"
#include "gui.hpp"

#include "pixcraft/server/world_module.hpp"
#include "pixcraft/server/world.hpp"
#include "pixcraft/server/chunk.hpp"

namespace PixCraft {
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
		
		static constexpr float PLAYER_REACH = 5.0f;
		
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
		ParticleRenderer particleRenderer;
		Hotbar hotbar;
		TextRenderer textRenderer;
		
		ShaderProgram cursorProgram;
		IndexBuffer<glm::vec2> cursorBuffer;
		ShaderProgram colorOverlayProgram;
		VertexBuffer<glm::vec2> colorOverlayBuffer;
		ShaderProgram blockOverlayProgram;
		IndexBuffer<glm::vec3> blockOverlayBuffer;
		
		Button testButton;
		
		int frameNo;
		int FPS;
		
		void setAntialiasing(bool enabled);
		void setRenderDistance(int renderDist);
		
		void executeCommand();
		void update(float dt);
		void render();
		
		friend void windowResizeCallback(GLFWwindow* window, int width, int height);
	};
}
