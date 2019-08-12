#pragma once

#define TEX(name) (TextureManager::name)
#define REQ_BLOCK_TEX(name, filename) const unsigned int name = requireBlockTexture(filename)
#define REQ_TEX(name, filename) const unsigned int name = requireTexture(filename)

namespace TextureManager {
	unsigned int requireBlockTexture(const char* filename);
	unsigned int requireTexture(const char* filename);

	REQ_BLOCK_TEX(PLACEHOLDER, "placeholder");
	REQ_BLOCK_TEX(STONE, "stone");
	REQ_BLOCK_TEX(DIRT, "dirt");
	REQ_BLOCK_TEX(GRASS_TOP, "grass_top");
	REQ_BLOCK_TEX(GRASS_SIDE, "grass_side");
	REQ_BLOCK_TEX(TRUNK_SIDE, "trunk_side");
	REQ_BLOCK_TEX(TRUNK_INSIDE, "trunk_inside");
	REQ_BLOCK_TEX(LEAVES, "leaves");
	REQ_BLOCK_TEX(WATER, "water");
	
	void loadTextures();
	void bindBlockTextureArray();
};
