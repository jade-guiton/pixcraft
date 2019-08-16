#pragma once

#include <cstdint>
#include <unordered_map>

#include "glfw.hpp"
#include "glm.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "utf8.h"

#include "texture_atlas.hpp"


struct Glyph {
	unsigned int atlasId;
	glm::ivec2 size;
	glm::ivec2 bearing;
	int32_t advance;
};


class TextRenderer {
public:
	void init();
	
	~TextRenderer();
	
	void setViewport(int width, int height);
	
	void renderText(std::string str, float x, float y, float scale, glm::vec3 color);
	
private:
	FT_Library ft;
	FT_Face face;
	TextureAtlas glyphAtlas;
	std::unordered_map<uint32_t, Glyph> characters;
	
	int winWidth, winHeight;
	GlId program, VAO, VBO;
	int fontHeight;
	
	void prerenderCharacter(uint32_t c);
};
