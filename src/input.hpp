#pragma once

#include <unordered_set>
#include <tuple>

#include "glfw.hpp"
#include "glm.hpp"

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
};
