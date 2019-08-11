#pragma once

#include <vector>
#include <cstddef>

#include "glfw.hpp"
#include "glm.hpp"

struct FaceData {
	float offsetX;
	float offsetY;
	float offsetZ;
	uint8_t side;
	uint32_t texId;
} __attribute__((packed));
// ^^^ It works without the __attribute__, but adding it allows sending less data to the GPU

class FaceRenderer;

class FaceBuffer {
public:
	FaceBuffer();
	~FaceBuffer();
	void init(FaceRenderer& faceRenderer, int capacity);
	bool isInitialized();
	
	std::vector<FaceData> faces;
	
	void prerender();
	void eraseFaces(int8_t x, int8_t y, int8_t z);
	void erasePlaneX(int8_t x);
	void erasePlaneZ(int8_t z);
	
	void render();
	
private:
	GlId VAO, VBO;
	size_t capacity;
};

struct RenderParams {
	float skyColor[3];
	
	bool applyView;
	bool applyFog;
	
	float fogStart;
	float fogEnd;
};

class FaceRenderer {
public:
	FaceRenderer();
	void init();
	
	void bindFaceAttributes();
	
	void setParams(RenderParams params);
	void startRendering(glm::mat4 proj, glm::mat4 view, RenderParams params);
	void render(FaceBuffer& buffer, glm::mat4 model);
	void stopRendering();
	
private:
	GlId program, faceVBO;
};
