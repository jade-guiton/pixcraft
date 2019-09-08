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
		void clearJustClicked();
		glm::vec2 getMouseMovement();
		
		void keyPressed(int key);
		bool justPressed(int key);
		void clearJustPressed();
		std::tuple<int,int,bool,bool> getMovementKeys();
		
		void inputCharacter(uint32_t codepoint);
		std::string retrieveInputBuffer();
		
		void scrolled(int offset);
		int justScrolled();
		void clearJustScrolled();
		
	private:
		static constexpr float mouseSensitivity = 0.005f;
		
		GLFWwindow* window;
		
		bool _capturingMouse;
		double oldMouseX, oldMouseY;
		
		std::unordered_set<int> _justPressed;
		bool _justClicked[2];
		
		int _justScrolled;
		
		std::string inputBuffer;
	};
}
