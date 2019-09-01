
#include <iostream>

namespace PixCraft {
	template<std::size_t N>
	void ShaderProgram::setUniformArray(const char* name, std::array<glm::mat3, N> array) {
		glUniformMatrix3fv(glGetUniformLocation(programId, name), N, GL_FALSE, glm::value_ptr(array[0]));
	}


	template<typename... Ts>
	VertexBuffer<Ts...>::VertexBuffer() : vboId(0), _vertexCount(0) {}

	template<typename... Ts>
	VertexBuffer<Ts...>::~VertexBuffer() {
		if(vboId == 0) return;
		glDeleteBuffers(1, &vboId);
	}

	template<typename... Ts>
	void VertexBuffer<Ts...>::loadData(const void* data, size_t vertexCount, GLenum usage) {
		_vertexCount = vertexCount;
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glBufferData(GL_ARRAY_BUFFER, vertexSize*vertexCount, data, usage);
	}

	template<typename... Ts>
	size_t VertexBuffer<Ts...>::vertexCount() { return _vertexCount; }

	template<typename... Ts>
	void VertexBuffer<Ts...>::updateData(const void* data, size_t vertexCount) {
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize*vertexCount, data);
	}

	template<typename... Ts>
	void VertexBuffer<Ts...>::initLocation(int location, size_t vertexSize2) {
		vertexSize = vertexSize2;
		VertexArray::init();
	}

	template<typename... Ts>
	void VertexBuffer<Ts...>::genBuffers() {
		glGenBuffers(1, &vboId);
	}

	template<typename... Ts>
	void VertexBuffer<Ts...>::setVAO() {
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		setAttributes();
	}

	template<typename... Ts>
	void VertexBuffer<Ts...>::setAttributes() {}


	template<typename T, typename... Ts>
	VertexBuffer<T, Ts...>::VertexBuffer() : location(-1), offset(0) {}

	template<typename T, typename... Ts>
	template<typename... Args>
	void VertexBuffer<T, Ts...>::init(size_t offset2, Args... offsets) {
		initLocation(0, offset2, offsets...);
	}

	template<typename T, typename... Ts>
	template<typename... Args>
	void VertexBuffer<T, Ts...>::initLocation(int location2, size_t offset2, Args... offsets) {
		location = location2;
		offset = offset2;
		VertexBuffer<Ts...>::initLocation(location2 + 1, offsets...);
	}

	template<typename T, typename... Ts>
	void VertexBuffer<T, Ts...>::setAttributes() {
		setAttributePointer<T>(location, offset, VertexBuffer::vertexSize);
		glEnableVertexAttribArray(location);
		VertexBuffer<Ts...>::setAttributes();
	}


	template<typename... Ts>
	IndexBuffer<Ts...>::IndexBuffer() : eboId(0), _indexCount(0) {}

	template<typename... Ts>
	IndexBuffer<Ts...>::~IndexBuffer() {
		if(eboId == 0) return;
		glDeleteBuffers(1, &eboId);
	}

	template<typename... Ts>
	void IndexBuffer<Ts...>::loadIndices(const void* data, size_t indexCount) {
		_indexCount = indexCount;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount*sizeof(unsigned int), data, GL_STATIC_DRAW);
	}

	template<typename... Ts>
	size_t IndexBuffer<Ts...>::indexCount() { return _indexCount; }

	template<typename... Ts>
	void IndexBuffer<Ts...>::genBuffers() {
		VertexBuffer<Ts...>::genBuffers();
		glGenBuffers(1, &eboId);
	}

	template<typename... Ts>
	void IndexBuffer<Ts...>::setVAO() {
		VertexBuffer<Ts...>::setVAO();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
	}
}
