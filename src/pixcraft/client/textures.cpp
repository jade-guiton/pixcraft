#include "textures.hpp"

#include <vector>
#include <string>
#include <stdexcept>

#include <iostream>

#include <stb_image.h>
#include "glfw.hpp"
#include "../util/glm.hpp"

namespace PixCraft::TextureManager {
	namespace {
		std::vector<std::string> blockTextureFiles;
		GlId blockTextureArray;
		
		std::vector<std::string> otherTextureFiles;
		std::vector<GlId> otherTextures;
		std::vector<glm::uvec2> otherTextureDim;
		
		TexId requireBlockTexture(const char* filename) {
			blockTextureFiles.push_back(std::string(filename));
			return blockTextureFiles.size() - 1;
		}
		
		TexId requireTexture(const char* filename) {
			otherTextureFiles.push_back(std::string(filename));
			return otherTextureFiles.size() - 1;
		}
	}
	
	const TexId PLACEHOLDER = requireBlockTexture("placeholder");
	const TexId STONE = requireBlockTexture("stone");
	const TexId DIRT = requireBlockTexture("dirt");
	const TexId GRASS_SIDE = requireBlockTexture("grass_side");
	const TexId GRASS_TOP = requireBlockTexture("grass_top");
	const TexId TRUNK_SIDE = requireBlockTexture("trunk_side");
	const TexId TRUNK_INSIDE = requireBlockTexture("trunk_inside");
	const TexId LEAVES = requireBlockTexture("leaves");
	const TexId WATER = requireBlockTexture("water");
	const TexId PLANKS = requireBlockTexture("planks");
	
	const TexId SLIME = requireTexture("entity/slime");
	
	const TexId LOGO = requireTexture("gui/logo");
	const TexId BUTTON = requireTexture("gui/button");
	
	void loadTextures() {
		glGenTextures(1, &blockTextureArray);
		glBindTexture(GL_TEXTURE_2D_ARRAY, blockTextureArray);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, BLOCK_TEX_SIZE, BLOCK_TEX_SIZE,
			blockTextureFiles.size(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		
		for(unsigned int i = 0; i < blockTextureFiles.size(); ++i) {
			std::string filename = "res/block/" + blockTextureFiles[i] + ".png";
			unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
			if(!data) throw std::runtime_error("Failed to load block texture");
			if(width != BLOCK_TEX_SIZE || height != BLOCK_TEX_SIZE) throw std::runtime_error("Block texture has incorrect dimensions");
			if(nrChannels == 3)
				glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, BLOCK_TEX_SIZE, BLOCK_TEX_SIZE, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
			else if(nrChannels == 4)
				glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, BLOCK_TEX_SIZE, BLOCK_TEX_SIZE, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
		
		otherTextures.assign(otherTextureFiles.size(), 0);
		glGenTextures(otherTextureFiles.size(), otherTextures.data());
		
		for(unsigned int i = 0; i < otherTextureFiles.size(); ++i) {
			std::string filename = "res/" + otherTextureFiles[i] + ".png";
			unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
			if(!data) throw std::runtime_error("Failed to load texture");
			
			glBindTexture(GL_TEXTURE_2D, otherTextures[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			
			if(nrChannels == 3)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			else if(nrChannels == 4)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			
			stbi_image_free(data);
			
			otherTextureDim.emplace_back(width, height);
		}
		checkGlErrors("texture loading");
	}
	
	void bindBlockTextureArray() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, blockTextureArray);
	}
	
	void bindOtherTextures(TexId texId) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, otherTextures[texId]);
	}
	
	int getTextureWidth(TexId texId) { return otherTextureDim[texId].x; }
	int getTextureHeight(TexId texId) { return otherTextureDim[texId].y; }
}
