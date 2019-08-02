#pragma once

#include <stdexcept>
#include <string>
#include <cstddef>
#include <vector>
#include <unordered_map>

#include <iostream>

#include "glfw.hpp"
#include "glm.hpp"
#include <stb_image.h>

#include "shaders.hpp"
#include "world.hpp"
#include "blocks.hpp"
#include "chunk.hpp"
#include "util.hpp"

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
	void init(FaceRenderer& faceRenderer, int capacity);
	bool isInitialized();
	
	void load(std::vector<FaceData>& faces);
	
	void render();
	
private:
	GlId VAO, VBO;
	int capacity, faceCount;
};

struct RenderParams {
	float skyColor[3];
	
	bool fog;
	float fogStart;
	float fogEnd;
};

class FaceRenderer {
public:
	FaceRenderer();
	void init();
	
	void bindFaceAttributes();
	
	void startRendering(glm::mat4 proj, glm::mat4 view, RenderParams params);
	void render(FaceBuffer& buffer, glm::mat4 model);
	void stopRendering();
	
private:
	GlId program, faceVBO, textureArray;
};
