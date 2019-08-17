#pragma once

#include <vector>

#include "glfw.hpp"

struct Rect {
	int left, right, bottom, top, width, height;
};

class TextureAtlas {
public:
	void init(int width, int height);
	
	unsigned int addTexture(int width, int height, void* data);
	//void overlay(unsigned int textureId, int overlayWidth, int overlayHeight, void* data);
	
	float getL(unsigned int textureId);
	float getR(unsigned int textureId);
	float getT(unsigned int textureId);
	float getB(unsigned int textureId);
	
	void bind();
	
private:
	GlId texture;
	int width, height;
	
	std::vector<Rect> freeSpace;
	std::vector<Rect> textures;
};
