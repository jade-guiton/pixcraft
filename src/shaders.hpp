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
	void setUniformArray(const char* name, std::array<glm::mat3, N> array);
	
	void use();
	void unuse();
	
private:
	GlId programId;
};


class VertexArray {
public:
	void bind();
	void unbind();
	
protected:
	void init();
	
	virtual void genBuffers();
	virtual void setVAO();
	
private:
	GlId vaoId;
};

template<typename... Ts>
class VertexBuffer : public VertexArray {
public:
	void loadData(const void* data, size_t vertexCount, GLenum usage);
	
protected:
	size_t vertexSize;
	
	void initLocation(int location, size_t vertexSize);
	
	void genBuffers() override;
	void setVAO() override;
	
	virtual void setAttributes();

private:
	GlId vboId;
};

template<typename T>
void setAttributePointer(int location, size_t offset, size_t totalSize);

template<typename T, typename... Ts>
class VertexBuffer<T, Ts...> : public VertexBuffer<Ts...> {
public:
	template<typename... Args>
	void init(size_t offset, Args... offsets);
	
protected:
	template<typename... Args>
	void initLocation(int location, size_t offset, Args... offsets);
	
	void setAttributes() override;
	
private:
	int location;
	size_t offset;
};

template<typename... Ts>
class IndexBuffer : public VertexBuffer<Ts...> {
public:
	using VertexBuffer<Ts...>::init;
	
	void loadIndices(const void* data, size_t indexCount);
	
protected:
	void genBuffers() override;
	void setVAO() override;
	
private:
	GlId eboId;
};

#include "shaders_impl.hpp"
