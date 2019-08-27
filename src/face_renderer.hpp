#pragma once

#include <vector>
#include <cstddef>

#include "glfw.hpp"
#include "glm.hpp"

#include "shaders.hpp"
#include "textures.hpp"

struct FaceData {
	float offsetX;
	float offsetY;
	float offsetZ;
	uint8_t side;
	TexId texId;
} __attribute__((packed));
// ^^^ It works without the __attribute__, but adding it allows sending less data to the GPU

class FaceRenderer;

class FaceBuffer {
public:
	void init(FaceRenderer& faceRenderer, int capacity);
	bool isInitialized();
	
	std::vector<FaceData> faces;
	
	void prerender();
	void eraseFaces(int8_t x, int8_t y, int8_t z);
	void erasePlaneX(int8_t x);
	void erasePlaneZ(int8_t z);
	
	void render();
	
private:
	VertexBuffer<glm::vec3, uint8_t, uint32_t> buffer;
};

class FaceRenderer {
public:
	FaceRenderer();
	void init();
	
	void setParams(RenderParams params);
	void startRendering(glm::mat4 proj, glm::mat4 view, RenderParams params);
	void render(FaceBuffer& buffer, glm::mat4 model);
	void stopRendering();
	
private:
	ShaderProgram program;
};
