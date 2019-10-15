#pragma once

#include <cstdint>
#include <unordered_map>
#include <array>

#include "glfw.hpp"
#include "pixcraft/util/glm.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H
#include "utf8.h"

#include "shaders.hpp"
#include "texture_atlas.hpp"

namespace PixCraft {
	struct GlyphData {
		unsigned int atlasId;
		glm::ivec2 size;
		glm::ivec2 bearing;
	};
	
	struct CharacterData {
		GlyphData glyphData;
		int32_t advance;
		int32_t height;
	};
	
	class TextRenderer {
	public:
		void init();
		
		void free();
		
		void setViewport(int width, int height);
		
		int getTextWidth(std::string str);
		int getTextHeight();
		
		void renderText(std::string str, float x, float y, glm::vec4 color);
		
	private:
		static const size_t QUAD_SIZE = 24;
		static const int BUFFER_SIZE = 32;
		
		FT_Library ft;
		FT_Stroker stroker;
		std::vector<FT_Face> faces;
		TextureAtlas glyphAtlas;
		std::unordered_map<uint32_t, CharacterData> characters;
		
		int winWidth, winHeight;
		ShaderProgram program;
		VertexBuffer<glm::vec2, glm::vec2> buffer;
		int fontHeight;
		int xHeight;
		
		void loadFont(const char* filename);
		
		void prerenderCharacter(uint32_t c);
		
		void renderGlyphData(float*& bufferIt, GlyphData& data, float x, float y);
	};
}
