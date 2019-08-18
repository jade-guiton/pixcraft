#pragma once

#include <cstdint>

typedef uint32_t TexId;

#define TEX(name) (TextureManager::name)

namespace TextureManager {
	void loadTextures();
	void bindBlockTextureArray();
	void bindOtherTextures(TexId texId);
	
	// Block textures
	extern const TexId PLACEHOLDER;
	extern const TexId STONE;
	extern const TexId DIRT;
	extern const TexId GRASS_SIDE;
	extern const TexId GRASS_TOP;
	extern const TexId TRUNK_SIDE;
	extern const TexId TRUNK_INSIDE;
	extern const TexId LEAVES;
	extern const TexId WATER;
	extern const TexId PLANKS;
	
	// Other textures
	extern const TexId SLIME;
};
