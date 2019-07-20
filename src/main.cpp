#include <iostream>
#include <stdexcept>

#include "glfw.hpp"

#include "client.hpp"

void glfwErrorCallback(int error, const char* desc) {
	std::cout << "GLFW error " << error << ": " << desc << std::endl;
}

int main() {
	glfwSetErrorCallback(glfwErrorCallback);
	glfwInit();
	
	try {
		GameClient client;
		client.mainLoop();
	} catch(std::runtime_error& err) {
		std::cout << "A runtime error occured: " << err.what() << std::endl;
		glfwTerminate();
		return 1;
	}
	
	glfwTerminate();
	return 0;
}
