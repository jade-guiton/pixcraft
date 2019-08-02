#pragma once

#include <vector>
#include <string>
#include <stdexcept>

#include <stb_image.h>
#include "glfw.hpp"

#define TEX(name) (TextureManager::name)
#define REQ_TEX(name, filename) const unsigned int name = require(filename)

namespace TextureManager {
	unsigned int require(const char* filename);

	REQ_TEX(PLACEHOLDER, "placeholder");
	REQ_TEX(STONE, "stone");
	REQ_TEX(DIRT, "dirt");
	REQ_TEX(GRASS_TOP, "grass_top");
	REQ_TEX(GRASS_SIDE, "grass_side");
	REQ_TEX(TRUNK_SIDE, "trunk_side");
	REQ_TEX(TRUNK_INSIDE, "trunk_inside");
	REQ_TEX(LEAVES, "leaves");
	
	void loadTextures();
	void bindTextureArray();
};
