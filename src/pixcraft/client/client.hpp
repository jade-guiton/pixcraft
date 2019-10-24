#pragma once

#include <memory>

#include "glfw.hpp"

#include "input.hpp"
#include "text.hpp"

namespace PixCraft {
	class GameClient;
	
	class GameState {
	public:
		GameState(GameClient& client);
		virtual ~GameState() = default;
		
		virtual void update(float dt) = 0;
		virtual void render(int winWidth, int winHeight) = 0;
		
	protected:
		GameClient& client;
	};
	
	class GameClient {
	public:
		GameClient();
		~GameClient();
		
		void run();
		void stop();
		
		InputManager& getInputManager();
		TextRenderer& getTextRenderer();
		
		void setViewportSize(int width, int height);
		void setGameState(GameState* gameState);
		
		int getFrameNo();
		int getFPS();
		
	private:
		static const int START_WIDTH = 800;
		static const int START_HEIGHT = 600;
		
		GLFWwindow* window;
		int width, height;
		
		InputManager input;
		TextRenderer textRenderer;
		
		std::unique_ptr<GameState> gameState;
		GameState* nextGameState;
		
		int frameNo;
		int FPS;
		bool fullscreen;
		int windowedWidth, windowedHeight;
		
		friend void windowResizeCallback(GLFWwindow* window, int width, int height);
	};
}
