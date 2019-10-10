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
		
		InputManager& getInputManager();
		TextRenderer& getTextRenderer();
		
		void setGameState(std::unique_ptr<GameState> gameState);
		
		int getFrameNo();
		int getFPS();
		
	private:
		static const int START_WIDTH = 800;
		static const int START_HEIGHT = 600;
		
		GLFWwindow* window;
		InputManager input;
		TextRenderer textRenderer;
		
		std::unique_ptr<GameState> gameState;
		
		int frameNo;
		int FPS;
		
		friend void windowResizeCallback(GLFWwindow* window, int width, int height);
	};
}
