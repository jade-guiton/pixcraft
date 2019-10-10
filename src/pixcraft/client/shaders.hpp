#pragma once

#include <array>

#include "glfw.hpp"
#include "pixcraft/util/glm.hpp"

namespace PixCraft {
	namespace ShaderSources {
		extern const char *blockVS, *blockGS, *blockFS;
		extern const char *entityVS, *entityFS;
		extern const char *particleVS, *particleFS;
		extern const char *menuBgVS, *menuBgFS;
		
		extern const char *cursorVS;
		extern const char *overlayVS;
		extern const char *blockOverlayVS;
		extern const char *guiVS;
		extern const char *textFS;
		extern const char *colorFS;
		extern const char *textureFS;
	}
	
	class ShaderProgram {
	public:
		ShaderProgram();
		
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
		VertexArray();
		virtual ~VertexArray();
		
		void bind();
		void unbind();
		bool isInitialized();
		
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
		VertexBuffer();
		virtual ~VertexBuffer();
		
		void loadData(const void* data, size_t vertexCount, GLenum usage);
		size_t vertexCount();
		
		void updateData(const void* data, size_t vertexCount);
		
	protected:
		size_t vertexSize;
		
		void initLocation(int location, size_t vertexSize);
		
		void genBuffers() override;
		void setVAO() override;
		
		virtual void setAttributes();

	private:
		GlId vboId;
		size_t _vertexCount;
	};
	
	template<typename T>
	void setAttributePointer(int location, size_t offset, size_t totalSize);
	
	template<typename T, typename... Ts>
	class VertexBuffer<T, Ts...> : public VertexBuffer<Ts...> {
	public:
		VertexBuffer();
		
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
		IndexBuffer();
		virtual ~IndexBuffer();
		
		using VertexBuffer<Ts...>::init;
		
		void loadIndices(const void* data, size_t indexCount);
		size_t indexCount();
		
	protected:
		void genBuffers() override;
		void setVAO() override;
		
	private:
		GlId eboId;
		size_t _indexCount;
	};
}

#include "shaders_impl.hpp"
