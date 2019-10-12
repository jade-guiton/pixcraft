#pragma once

#include <unordered_set>
#include <tuple>
#include <string>
#include <iterator>

#include "glfw.hpp"
#include "pixcraft/util/glm.hpp"

namespace PixCraft {
	class InputManager {
	public:
		InputManager();
		void init(GLFWwindow* window);
		
		void capturingMouse(bool capture);
		
		void mouseClicked(int button);
		bool justClicked(int button);
		glm::vec2 getMouseMovement();
		glm::ivec2 getMousePosition();
		
		void keyPressed(int key);
		bool justPressed(int key);
		std::tuple<int,int,bool,bool> getMovementKeys();
		
		void inputCharacter(uint32_t codepoint);
		std::string inputBuffer();
		
		void scrolled(int offset);
		int justScrolled();
		
		void clearAll();
		
	private:
		static constexpr float mouseSensitivity = 0.005f;
		
		GLFWwindow* window;
		
		bool _capturingMouse;
		double oldMouseX, oldMouseY;
		
		std::unordered_set<int> _justPressed;
		bool _justClicked[2];
		
		int _justScrolled;
		
		std::string _inputBuffer;
	};
}
