#include "block_renderer.hpp"

const float faceVertices[] = {
	-0.5f, -0.5f,  0.5f,  0.0, 0.0, // 0
	 0.5f, -0.5f,  0.5f,  1.0, 0.0, // 1
	-0.5f,  0.5f,  0.5f,  0.0, 1.0, // 2
	 0.5f,  0.5f,  0.5f,  1.0, 1.0  // 3
};
const unsigned int faceIndices[] = {
	0, 1, 3, 3, 2, 0
};

const glm::mat4 id = glm::mat4(1.0f);
const glm::mat4 sideTransforms[6] = {
	glm::mat4(1.0f),
	glm::rotate(id, TAU/4, glm::vec3(0.0f, 1.0f, 0.0f)),
	glm::rotate(id, TAU/2, glm::vec3(0.0f, 1.0f, 0.0f)),
	glm::rotate(id, -TAU/4, glm::vec3(0.0f, 1.0f, 0.0f)),
	glm::rotate(id, TAU/4, glm::vec3(1.0f, 0.0f, 0.0f)),
	glm::rotate(id, -TAU/4, glm::vec3(1.0f, 0.0f, 0.0f))
};

const size_t BLOCK_TEX_SIZE = 16;
const size_t BLOCK_TEX_COUNT = 5;
const char* textureFiles[BLOCK_TEX_COUNT] = {
	"placeholder", "stone", "dirt", "grass_side", "grass_top"
};

const FaceData faces[] = {
	{ 0.0f, 0.0f, 0.0f, 0, 3 },
	{ 0.0f, 0.0f, 0.0f, 1, 3 },
	{ 0.0f, 0.0f, 0.0f, 2, 3 },
	{ 0.0f, 0.0f, 0.0f, 3, 3 },
	{ 0.0f, 0.0f, 0.0f, 4, 2 },
	{ 0.0f, 0.0f, 0.0f, 5, 4 }
};

void BlockRenderer::init() {
	program = loadShaders();
	
	unsigned int faceVBO, faceEBO, chunkVBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &faceVBO);
	glGenBuffers(1, &faceEBO);
	glGenBuffers(1, &chunkVBO);
	
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, faceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(faceVertices), faceVertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3*sizeof(float)));
	glEnableVertexAttribArray(1);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faceIndices), faceIndices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, chunkVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(faces), faces, GL_STATIC_DRAW);
	size_t faceDataSize = sizeof(FaceData);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, faceDataSize, (void*) offsetof(FaceData, offsetX));
	glVertexAttribDivisor(2, 1);
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(3, 1, GL_UNSIGNED_BYTE, faceDataSize, (void*) offsetof(FaceData, side));
	glVertexAttribDivisor(3, 1);
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, faceDataSize, (void*) offsetof(FaceData, texId));
	glVertexAttribDivisor(4, 1);
	glEnableVertexAttribArray(4);
	
	glBindVertexArray(0);
	
	glGenTextures(1, &textureArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, BLOCK_TEX_SIZE, BLOCK_TEX_SIZE, BLOCK_TEX_COUNT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	for(size_t i = 0; i < BLOCK_TEX_COUNT; ++i) {
		std::string filename = "res/" + std::string(textureFiles[i]) + ".png";
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
		if(!data) throw std::runtime_error("Failed to load block texture");
		if(width != BLOCK_TEX_SIZE || height != BLOCK_TEX_SIZE) throw std::runtime_error("Block texture has incorrect dimensions");
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, BLOCK_TEX_SIZE, BLOCK_TEX_SIZE, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}

void BlockRenderer::render(glm::mat4 proj, glm::mat4 view) {
	glm::mat4 model = glm::mat4(1.0f);
	
	glUseProgram(program);
	
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(glGetUniformLocation(program, "sideTransforms"), 6, GL_FALSE, glm::value_ptr(sideTransforms[0]));
	
	glUniform1i(glGetUniformLocation(program, "tex"), 0);
	
	glUniform1f(glGetUniformLocation(program, "ambientLight"), 0.7);
	glUniform1f(glGetUniformLocation(program, "diffuseLight"), 0.3);
	glm::vec3 lightSrcDir = glm::normalize(glm::vec3(0.5f, 1.0f, 0.1f));
	glUniform3fv(glGetUniformLocation(program, "lightSrcDir"), 1, glm::value_ptr(lightSrcDir));
	
	glUniform4f(glGetUniformLocation(program, "fogColor"), 0.3f, 0.4f, 0.4f, 1.0f);
	glUniform1f(glGetUniformLocation(program, "fogStart"), 10);
	glUniform1f(glGetUniformLocation(program, "fogEnd"), 15);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 6);
	glBindVertexArray(0);
}
