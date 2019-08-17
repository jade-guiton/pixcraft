#include "texture_atlas.hpp"

#include <algorithm>
#include <stdexcept>
#include <cstddef>

#include <iostream>


const GLint internalFormat = GL_RG8;
const GLenum format = GL_RG;

Rect newRect(int left, int bottom, int width, int height) {
	return Rect { left, left + width, bottom, bottom + height, width, height };
}

void TextureAtlas::init(int width2, int height2) {
	width = width2; height = height2;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
		format, GL_UNSIGNED_BYTE, nullptr);
	
	freeSpace.push_back(newRect(0, 0, width, height));
}

unsigned int TextureAtlas::addTexture(int width, int height, void* data) {
	int width2 = width + 1;
	int height2 = height + 1;
	
	int bestGap = -1;
	size_t bestRectIdx;
	for(size_t i = 0; i < freeSpace.size(); ++i) {
		Rect freeRect = freeSpace[i];
		if(width2 <= freeRect.width && height2 <= freeRect.height) {
			int gap = std::min(freeRect.width - width2, freeRect.height - height2);
			if(bestGap == -1 || gap < bestGap) {
				bestGap = gap;
				bestRectIdx = i;
			}
		}
	}
	
	if(bestGap == -1)
		throw std::runtime_error("No more space in texture atlas.");
	
	Rect bestRect = freeSpace[bestRectIdx];
	freeSpace.erase(freeSpace.begin() + bestRectIdx);
	
	if(bestRect.width < bestRect.height) {
		freeSpace.push_back(newRect(bestRect.left, bestRect.bottom + height2, bestRect.width, bestRect.height - height2));
		freeSpace.push_back(newRect(bestRect.left + width2, bestRect.bottom, bestRect.width - width2, height2));
	} else {
		freeSpace.push_back(newRect(bestRect.left + width2, bestRect.bottom, bestRect.width - width2, bestRect.height));
		freeSpace.push_back(newRect(bestRect.left, bestRect.bottom + height2, width2, bestRect.height - height2));
	}
	
	Rect texture = newRect(bestRect.left + 1, bestRect.bottom + 1, width, height);
	textures.push_back(texture);
	
	glTexSubImage2D(GL_TEXTURE_2D, 0, texture.left, texture.bottom, texture.width, texture.height, format, GL_UNSIGNED_BYTE, data);
	checkGlErrors("rendering to glyph atlas");
	
	return textures.size() - 1;
}

float TextureAtlas::getL(unsigned int textureId) {
	return (float) textures[textureId].left / width;
}

float TextureAtlas::getR(unsigned int textureId) {
	return (float) textures[textureId].right / width;
}

float TextureAtlas::getB(unsigned int textureId) {
	return (float) textures[textureId].bottom / height;
}

float TextureAtlas::getT(unsigned int textureId) {
	return (float) textures[textureId].top / height;
}

void TextureAtlas::bind() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
}