#include "textures.hpp"

#include <vector>
#include <string>
#include <stdexcept>

#include <stb_image.h>
#include "glfw.hpp"

namespace TextureManager {
	namespace {
		const unsigned int BLOCK_TEX_SIZE = 16;
		std::vector<std::string> blockTextureFiles;
		GlId blockTextureArray;
		
		std::vector<std::string> otherTextureFiles;
		std::vector<GlId> otherTextures;
	}
	
	unsigned int requireBlockTexture(const char* filename) {
		blockTextureFiles.push_back(std::string(filename));
		return blockTextureFiles.size() - 1;
	}
	
	unsigned int requireTexture(const char* filename) {
		otherTextureFiles.push_back(std::string(filename));
		return otherTextureFiles.size() - 1;
	}
	
	void loadTextures() {
		glGenTextures(1, &blockTextureArray);
		glBindTexture(GL_TEXTURE_2D_ARRAY, blockTextureArray);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, BLOCK_TEX_SIZE, BLOCK_TEX_SIZE,
			blockTextureFiles.size(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		
		for(unsigned int i = 0; i < blockTextureFiles.size(); ++i) {
			std::string filename = "res/" + blockTextureFiles[i] + ".png";
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
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			
			if(nrChannels == 3)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			else if(nrChannels == 4)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glGenerateMipmap(GL_TEXTURE_2D);
			
			stbi_image_free(data);
		}
	}
	
	void bindBlockTextureArray() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, blockTextureArray);
	}
	
	void bindOtherTexture(unsigned int texId) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, otherTextures[texId]);
	}
}
