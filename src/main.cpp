#include <iostream>
#include <stdexcept>

#include "glfw.hpp"

#include "window.hpp"

void glfwErrorCallback(int error, const char* desc) {
	std::cout << "GLFW error " << error << ": " << desc << std::endl;
}

int main() {
	glfwSetErrorCallback(glfwErrorCallback);
	glfwInit();
	
	try {
		GameWindow window;
		while(!window.shouldClose()) {
			window.update();
		}
	} catch(std::runtime_error& err) {
		std::cout << "A runtime error occured: " << err.what() << std::endl;
		glfwTerminate();
		return 1;
	}
	
	glfwTerminate();
	return 0;
}
