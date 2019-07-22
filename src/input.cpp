#include "input.hpp"

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	InputManager& input = *((InputManager*) glfwGetWindowUserPointer(window));
	if(action == GLFW_PRESS) {
		input.keyPressed(key);
	}
}

void InputManager::init(GLFWwindow* newWindow) {
	window = newWindow;
	
	glfwSetWindowUserPointer(window, (void*) this);
	
	glfwSetKeyCallback(window, keyCallback);
}

void InputManager::capturingMouse(bool capturingMouse) {
	_capturingMouse = capturingMouse;
	glfwSetInputMode(window, GLFW_CURSOR, capturingMouse ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glfwSetCursorPos(window, width/2, height/2);
	if(capturingMouse) {
		oldMouseX = width/2;
		oldMouseY = height/2;
	}
}

void InputManager::keyPressed(int key) {
	_justPressed.insert(key);
}

bool InputManager::justPressed(int key) {
	return _justPressed.count(key) == 1;
}

void InputManager::clearJustPressed() {
	_justPressed.clear();
}


glm::vec2 InputManager::getMouseMovement() {
	double newMouseX, newMouseY;
	glfwGetCursorPos(window, &newMouseX, &newMouseY);
	glm::vec2 mvt(newMouseX - oldMouseX, newMouseY - oldMouseY);
	oldMouseX = newMouseX; oldMouseY = newMouseY;
	return mouseSensitivity * mvt;
}

glm::vec3 InputManager::getMovement() {
	int dx = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) - (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
	int dy = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) - (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
	int dz = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) - (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
	if(dx != 0 || dy != 0 || dz != 0) {
		return glm::normalize(glm::vec3(dx, dy, dz));
	} else {
		return glm::vec3(0, 0, 0);
	}
}
