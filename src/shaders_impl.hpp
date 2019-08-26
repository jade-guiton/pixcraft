
#include <iostream>

template<std::size_t N>
void ShaderProgram::setUniformArray(const char* name, std::array<glm::mat3, N> array) {
	glUniformMatrix3fv(glGetUniformLocation(programId, name), N, GL_FALSE, glm::value_ptr(array[0]));
}

template<typename... Ts>
void VertexBuffer<Ts...>::loadData(const void* data, size_t vertexCount, GLenum usage) {
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, vertexSize*vertexCount, data, usage);
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
