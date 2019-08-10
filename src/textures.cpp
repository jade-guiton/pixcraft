#include "textures.hpp"

#include <vector>
#include <string>
#include <stdexcept>

#include <stb_image.h>
#include "glfw.hpp"

namespace TextureManager {
	namespace {
		const unsigned int SIZE = 16;
		std::vector<std::string> textureFiles;
		GlId textureArray;
	}
	
	unsigned int require(const char* filename) {
		textureFiles.push_back(std::string(filename));
		return textureFiles.size() - 1;
	}
	
	void loadTextures() {
		glGenTextures(1, &textureArray);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, SIZE, SIZE, textureFiles.size(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		for(unsigned int i = 0; i < textureFiles.size(); i++) {
			std::string filename = "res/" + textureFiles[i] + ".png";
			unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
			if(!data) throw std::runtime_error("Failed to load block texture");
			if(width != SIZE || height != SIZE) throw std::runtime_error("Block texture has incorrect dimensions");
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, SIZE, SIZE, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	}
	
	void bindTextureArray() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
	}
}
