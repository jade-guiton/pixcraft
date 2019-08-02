#include "input.hpp"

InputManager::InputManager()
	: window(nullptr), _capturingMouse(false), oldMouseX(0), oldMouseY(0), _justPressed(), _justClicked {false, false},
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

void InputManager::init(GLFWwindow* newWindow) {
	window = newWindow;
	
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);
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

void InputManager::mouseClicked(int button) {
	_justClicked[button - 1] = true;
}

bool InputManager::justClicked(int button) {
	return _justClicked[button - 1];
}

void InputManager::clearJustClicked() {
	_justClicked[0] = false;
	_justClicked[1] = false;
}

glm::vec2 InputManager::getMouseMovement() {
	double newMouseX, newMouseY;
	glfwGetCursorPos(window, &newMouseX, &newMouseY);
	glm::vec2 mvt(newMouseX - oldMouseX, newMouseY - oldMouseY);
	oldMouseX = newMouseX; oldMouseY = newMouseY;
	return mouseSensitivity * mvt;
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

std::tuple<int,int,bool,bool> InputManager::getMovementKeys() {
	int dx = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) - (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
	int dz = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) - (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
	bool up = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
	bool down = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
	return std::tuple<int,int,bool,bool>(dx, dz, up, down);
}

void InputManager::scrolled(int offset) { _justScrolled += offset; }

int InputManager::justScrolled() { return _justScrolled; }
void InputManager::clearJustScrolled() { _justScrolled = 0; }
