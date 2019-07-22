#pragma once

#include <unordered_set>

#include "glfw.hpp"
#include "glm.hpp"

class InputManager {
public:
	void init(GLFWwindow* window);
	
	void capturingMouse(bool capture);
	
	void keyPressed(int key);
	bool justPressed(int key);
	void clearJustPressed();
	
	glm::vec2 getMouseMovement();
	glm::vec3 getMovement();
	
private:
	static constexpr float mouseSensitivity = 0.005f;
	
	GLFWwindow* window;
	
	bool _capturingMouse;
	double oldMouseX, oldMouseY;
	
	std::unordered_set<int> _justPressed;
};
