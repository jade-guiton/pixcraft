#include "glfw.hpp"

#include <iostream>

void checkGlErrors(const char* opDesc) {
	GLenum error;
	while((error = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL error " << std::hex << error << std::dec << " during " << opDesc << "!" << std::endl;
	}
}