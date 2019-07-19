#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <iostream>
#include <stdexcept>

#include "glfw.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders.hpp"

#define WINDOW_START_WIDTH 800
#define WINDOW_START_HEIGHT 600

class GameWindow {
public:
	GameWindow();
	~GameWindow();
	
	bool shouldClose();
	void update();
	
private:
	GLFWwindow* handle;
	
	int width;
	int height;
	
	unsigned int program;
	unsigned int VAO;
	
	friend void windowResizeCallback(GLFWwindow* handle, int width, int height);
};

#endif
