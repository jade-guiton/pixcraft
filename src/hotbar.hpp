#pragma once

#include <vector>
#include <cstdint>

#include "glfw.hpp"
#include "glm.hpp"

#include "blocks.hpp"
#include "face_renderer.hpp"

class Hotbar {
public:
	Hotbar(FaceRenderer& renderer);
	void init();
	
	void setBlock(BlockId blockId);
	
	void render();
	
private:
	FaceRenderer& faceRenderer;
	FaceBuffer buffer;
};
