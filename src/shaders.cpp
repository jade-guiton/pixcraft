#include "shaders.hpp"

#include <iostream>

void checkShaderStatus(GlId shader, bool isProgram, const char* opDesc) {
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

GlId loadShaders(const char* vertexSrc, const char* fragmentSrc) {
	GlId vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
	glCompileShader(vertexShader);
	checkShaderStatus(vertexShader, false, "Vertex shader compilation");
	
	GlId fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSrc, nullptr);
	glCompileShader(fragmentShader);
	checkShaderStatus(fragmentShader, false, "Fragment shader compilation");
	
	GlId shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	checkShaderStatus(shaderProgram, true, "Shader program linking");
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	return shaderProgram;
}

GlId loadBlockProgram() {
	return loadShaders(blockVertexShaderSource, blockFragmentShaderSource);
}

GlId loadCursorProgram() {
	return loadShaders(cursorVertexShaderSource, colorFragmentShaderSource);
}

GlId loadColorOverlayProgram() {
	return loadShaders(overlayVertexShaderSource, colorFragmentShaderSource);
}

GlId loadEntityProgram() {
	return loadShaders(entityVertexShaderSource, entityFragmentShaderSource);
}

GlId loadTextProgram() {
	return loadShaders(textVertexShaderSource, textFragmentShaderSource);
}
