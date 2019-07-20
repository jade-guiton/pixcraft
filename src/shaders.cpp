#include "shaders.hpp"

void checkShaderStatus(unsigned int shader, bool isProgram, const char* opDesc) {
	int succ;
	char infoLog[512];
	if(isProgram)
		glGetProgramiv(shader, isProgram ? GL_LINK_STATUS : GL_COMPILE_STATUS, &succ);
	else
		glGetShaderiv(shader, isProgram ? GL_LINK_STATUS : GL_COMPILE_STATUS, &succ);
	if(!succ) {
		if(isProgram)
			glGetProgramInfoLog(shader, 512, nullptr, infoLog);
		else
			glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cout << opDesc << " error:\n" << infoLog << std::endl;
		throw std::runtime_error("Loading shaders failed.");
	}
}

unsigned int loadShaders() {
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);
	checkShaderStatus(vertexShader, false, "Vertex shader compilation");
	
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);
	checkShaderStatus(fragmentShader, false, "Fragment shader compilation");
	
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	checkShaderStatus(shaderProgram, true, "Shader program linking");
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	return shaderProgram;
}
