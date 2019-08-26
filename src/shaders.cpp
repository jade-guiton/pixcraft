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

void ShaderProgram::init(const char* vertexSrc, const char* geometrySrc, const char* fragmentSrc) {
	programId = glCreateProgram();
	
	GlId vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
	glCompileShader(vertexShader);
	glAttachShader(programId, vertexShader);
	checkShaderStatus(vertexShader, false, "Vertex shader compilation");
	
	GlId geometryShader;
	if(geometrySrc != nullptr) {
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryShader, 1, &geometrySrc, NULL);
		glCompileShader(geometryShader);
		glAttachShader(programId, geometryShader);
		checkShaderStatus(geometryShader, false, "Geometry shader compilation");
	}
	
	GlId fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSrc, nullptr);
	glCompileShader(fragmentShader);
	glAttachShader(programId, fragmentShader);
	checkShaderStatus(fragmentShader, false, "Fragment shader compilation");
	
	glLinkProgram(programId);
	checkShaderStatus(programId, true, "Shader program linking");
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if(geometrySrc != nullptr)
		glDeleteShader(geometryShader);
}

void ShaderProgram::init(const char* vertexSrc, const char* fragmentSrc) {
	init(vertexSrc, nullptr, fragmentSrc);
}

void ShaderProgram::setUniform(const char* name, bool val) {
	glUniform1i(glGetUniformLocation(programId, name), val);
}
void ShaderProgram::setUniform(const char* name, uint32_t val) {
	glUniform1i(glGetUniformLocation(programId, name), val);
}
void ShaderProgram::setUniform(const char* name, float val) {
	glUniform1f(glGetUniformLocation(programId, name), val);
}
void ShaderProgram::setUniform(const char* name, float x, float y) {
	glUniform2f(glGetUniformLocation(programId, name), x, y);
}
void ShaderProgram::setUniform(const char* name, glm::vec3 val) {
	glUniform3fv(glGetUniformLocation(programId, name), 1, glm::value_ptr(val));
}
void ShaderProgram::setUniform(const char* name, float r, float g, float b) {
	glUniform3f(glGetUniformLocation(programId, name), r, g, b);
}
void ShaderProgram::setUniform(const char* name, float r, float g, float b, float a) {
	glUniform4f(glGetUniformLocation(programId, name), r, g, b, a);
}
void ShaderProgram::setUniform(const char* name, glm::mat4& val) {
	glUniformMatrix4fv(glGetUniformLocation(programId, name), 1, GL_FALSE, glm::value_ptr(val));
}

void ShaderProgram::use() {
	glUseProgram(programId);
}
void ShaderProgram::unuse() {
	glUseProgram(0);
}
