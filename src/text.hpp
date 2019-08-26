#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "glfw.hpp"
#include "glm.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H
#include "utf8.h"

#include "shaders.hpp"
#include "texture_atlas.hpp"


struct GlyphData {
	unsigned int atlasId;
	glm::ivec2 size;
	glm::ivec2 bearing;
};

struct CharacterData {
	GlyphData glyphData;
	int32_t advance;
};

class TextRenderer {
public:
	void init();
	
	void free();
	
	void setViewport(int width, int height);
	
	void renderText(std::string str, float x, float y, float scale, glm::vec3 color);
	
private:
	static const int FONT_COUNT = 4;
	
	FT_Library ft;
	FT_Stroker stroker;
	FT_Face faces[FONT_COUNT];
	TextureAtlas glyphAtlas;
	std::unordered_map<uint32_t, CharacterData> characters;
	
	int winWidth, winHeight;
	ShaderProgram program;
	GlId VAO, VBO;
	int fontHeight;
	
	void loadFont(size_t priority, const char* filename);
	
	void prerenderCharacter(uint32_t c);
	
	void renderGlyphData(GlyphData& data, float x, float y, float scale);
};
