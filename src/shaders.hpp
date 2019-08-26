#pragma once

#include <array>

#include "glfw.hpp"
#include "glm.hpp"

namespace ShaderSources {
	extern const char *blockVS, *blockGS, *blockFS;
	extern const char *entityVS, *entityFS;
	extern const char *textVS, *textFS;
	
	extern const char *cursorVS;
	extern const char *overlayVS;
	extern const char *blockOverlayVS;
	extern const char *colorFS;
}

class ShaderProgram {
public:
	void init(const char* vertexSrc, const char* geometrySrc, const char* fragmentSrc);
	void init(const char* vertexSrc, const char* fragmentSrc);
	
	void setUniform(const char* name, bool val);
	void setUniform(const char* name, uint32_t val);
	void setUniform(const char* name, float val);
	void setUniform(const char* name, float x, float y);
	void setUniform(const char* name, glm::vec3 val);
	void setUniform(const char* name, float r, float g, float b);
	void setUniform(const char* name, float r, float g, float b, float a);
	void setUniform(const char* name, glm::mat4& val);
	
	template<std::size_t N>
	void setUniformArray(const char* name, std::array<glm::mat3, N> array) {
		glUniformMatrix3fv(glGetUniformLocation(programId, name), N, GL_FALSE, glm::value_ptr(array[0]));
	}
	
	void use();
	void unuse();
	
private:
	GlId programId;
};

/*
GlId loadBlockProgram();
GlId loadCursorProgram();
GlId loadColorOverlayProgram();
GlId loadEntityProgram();
GlId loadTextProgram();
GlId loadBlockOverlayProgram();
*/
