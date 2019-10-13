#include "input.hpp"

#include "utf8.h"

#include "client.hpp"

using namespace PixCraft;

InputManager::InputManager()
	: window(nullptr), _capturingMouse(false), oldMousePos(0, 0), _justPressed(), _justClicked {false, false},
	  _justScrolled(0) { }

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	GameClient& client = *((GameClient*) glfwGetWindowUserPointer(window));
	InputManager& input = client.getInputManager();
	if(action == GLFW_PRESS) {
		input.keyPressed(key);
	}
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	GameClient& client = *((GameClient*) glfwGetWindowUserPointer(window));
	InputManager& input = client.getInputManager();
	button = (button == GLFW_MOUSE_BUTTON_1) ? 1 : ((button == GLFW_MOUSE_BUTTON_2) ? 2 : 0);
	if(action == GLFW_PRESS && button != 0) {
		input.mouseClicked(button);
	}
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	GameClient& client = *((GameClient*) glfwGetWindowUserPointer(window));
	InputManager& input = client.getInputManager();
	if(yoffset != 0) {
		input.scrolled(yoffset);
	}
}

void charCallback(GLFWwindow* window, unsigned int codepoint) {
	GameClient& client = *((GameClient*) glfwGetWindowUserPointer(window));
	InputManager& input = client.getInputManager();
	input.inputCharacter(codepoint);
}

void InputManager::init(GLFWwindow* newWindow) {
	window = newWindow;
	
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetCharCallback(window, charCallback);
}

void InputManager::capturingMouse(bool capturingMouse) {
	if(capturingMouse != _capturingMouse) {
		_capturingMouse = capturingMouse;
		glfwSetInputMode(window, GLFW_CURSOR, capturingMouse ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		glfwSetCursorPos(window, width/2, height/2);
		oldMousePos = glm::ivec2(width/2, height/2);
	}
}

void InputManager::mouseClicked(int button) {
	_justClicked[button - 1] = true;
}

bool InputManager::justClicked(int button) {
	return _justClicked[button - 1];
}

glm::vec2 InputManager::getMouseMovement() {
	glm::ivec2 newMousePos = getMousePosition();
	glm::vec2 mvt = newMousePos - oldMousePos;
	oldMousePos = newMousePos;
	return mouseSensitivity * mvt;
}

glm::ivec2 InputManager::getMousePosition() {
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return glm::ivec2((int) mouseX - width/2, height/2 - (int) mouseY);
}


void InputManager::keyPressed(int key) {
	_justPressed.insert(key);
}

bool InputManager::justPressed(int key) {
	return _justPressed.count(key) == 1;
}

std::tuple<int,int,bool,bool> InputManager::getMovementKeys() {
	int dx = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) - (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
	int dz = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) - (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
	bool up = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
	bool down = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
	return std::tuple<int,int,bool,bool>(dx, dz, up, down);
}

void InputManager::inputCharacter(uint32_t codepoint) {
	utf8::append(codepoint, std::back_inserter(_inputBuffer));
}

std::string InputManager::inputBuffer() {
	return _inputBuffer;
}

void InputManager::scrolled(int offset) { _justScrolled += offset; }

int InputManager::justScrolled() { return _justScrolled; }

void InputManager::clearAll() {
	_justClicked[0] = false;
	_justClicked[1] = false;
	_justPressed.clear();
	_justScrolled = 0;
	_inputBuffer.clear();
}
