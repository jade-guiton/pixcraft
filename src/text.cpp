#include "text.hpp"

#include <utility>
#include <string>
#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <cmath>

#include "shaders.hpp"


void TextRenderer::init() {
	if(FT_Init_FreeType(&ft))
		throw std::runtime_error("Failed to initialize FreeType");
	
	FT_Stroker_New(ft, &stroker);
	FT_Stroker_Set(stroker, 64 * 3/2, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
	
	loadFont(0, "res/font/NotoSans-Regular.ttf");
	loadFont(1, "res/font/NotoEmoji-Regular.ttf");
	loadFont(2, "res/font/NotoSansCJKjp-Regular.otf");
	loadFont(3, "res/font/LastResort.ttf");
	
	fontHeight = 16;
	for(int i = 0; i < FONT_COUNT; ++i) {
		FT_Set_Pixel_Sizes(faces[i], 0, fontHeight);
	}
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
	
	glyphAtlas.init(512, 512);
	
	// Preload ASCII characters at least
	glyphAtlas.bind();
	for(uint32_t c = 0x20; c < 0x7F; ++c) {
		prerenderCharacter(c);
	}
	
	program = loadTextProgram();
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 4, nullptr, GL_DYNAMIC_DRAW);
	
	size_t vertexSize = 4 * sizeof(float);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, vertexSize, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexSize, (void*) (2*sizeof(float)));
	glEnableVertexAttribArray(1);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	checkGlErrors("text renderer initialization");
}

void TextRenderer::free() {
	for(int i = 0; i < FONT_COUNT; ++i) {
		FT_Done_Face(faces[i]);
	}
	FT_Done_FreeType(ft);
}

void TextRenderer::setViewport(int width, int height) {
	winWidth = width; winHeight = height;
}

void TextRenderer::renderText(std::string str, float startX, float startY, float scale, glm::vec3 color, bool outline) {
	glUseProgram(program);
	glBindVertexArray(VAO);
	
	glyphAtlas.bind();
	
	glUniform2f(glGetUniformLocation(program, "winSize"), winWidth, winHeight);
	glUniform1i(glGetUniformLocation(program, "tex"), 0);
	
	if(outline)
		renderTextOnce(str, startX, startY, scale, glm::vec3(1.0) - color, true);
	renderTextOnce(str, startX, startY, scale, color, false);
}

void TextRenderer::loadFont(size_t priority, const char* filename) {
	if(FT_New_Face(ft, filename, 0, &faces[priority])) {
		std::stringstream errorMsg;
		errorMsg << "Failed to load font " << filename << "." << std::endl;
		throw std::runtime_error(errorMsg.str());
	}
}

GlyphData TextRenderer::prerenderGlyph(FT_Glyph glyph) {
	FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, true);
	FT_BitmapGlyph bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
	
	glm::ivec2 size(bitmapGlyph->bitmap.width, bitmapGlyph->bitmap.rows);
	unsigned int atlasId = glyphAtlas.addTexture(size.x, size.y, bitmapGlyph->bitmap.buffer);
	
	return GlyphData {
		atlasId,
		size,
		glm::ivec2(bitmapGlyph->left, bitmapGlyph->top)
	};
}

void TextRenderer::prerenderCharacter(uint32_t cp) {
	int i = 0;
	FT_UInt glyphIdx;
	while(i < FONT_COUNT && (glyphIdx = FT_Get_Char_Index(faces[i], cp)) == 0) {
		++i;
	}
	
	if(i == FONT_COUNT || FT_Load_Glyph(faces[i], glyphIdx, FT_LOAD_DEFAULT)) {
		std::stringstream errorMsg;
		errorMsg << "Failed to load glyph for U+" << std::hex << cp << "." << std::endl;
		throw std::runtime_error(errorMsg.str());
	}
	
	FT_GlyphSlot glyphSlot = faces[i]->glyph;
	int32_t advanceX = glyphSlot->advance.x;
	
	FT_Glyph glyph;
	FT_Get_Glyph(glyphSlot, &glyph);
	FT_Glyph outlineGlyph = glyph;
	FT_Glyph_StrokeBorder(&outlineGlyph, stroker, false, false);
	
	GlyphData glyphData = prerenderGlyph(glyph);
	GlyphData outlineGlyphData = prerenderGlyph(outlineGlyph);
	
	characters[cp] = CharacterData {
		glyphData,
		outlineGlyphData,
		advanceX
	};
}

void TextRenderer::renderGlyphData(GlyphData& data, float x, float y, float scale) {
	float l = glyphAtlas.getL(data.atlasId);
	float r = glyphAtlas.getR(data.atlasId);
	float t = glyphAtlas.getT(data.atlasId);
	float b = glyphAtlas.getB(data.atlasId);
	
	float w = data.size.x * scale;
	float h = data.size.y * scale;
	float xpos = x + data.bearing.x * scale;
	float ypos = y - data.bearing.y * scale;
	
	float vertices[4][4] = {
		{ xpos,     ypos,       l, b },
		{ xpos,     ypos + h,   l, t },
		{ xpos + w, ypos,       r, b },
		{ xpos + w, ypos + h,   r, t }
	};
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void TextRenderer::renderTextOnce(std::string str, float startX, float startY, float scale, glm::vec3 color, bool outline) {
	glUniform3f(glGetUniformLocation(program, "textColor"), color.r, color.g, color.b);
	
	float x = startX;
	float y = startY;
	float lineHeight = round(fontHeight * 1.25);
	
	std::string::const_iterator it = str.begin();
	std::string::const_iterator end = str.end();
	uint32_t cp;
	
	while(it != end) {
		cp = utf8::next(it, end);
		if(cp == '\n') {
			x = startX;
			y += lineHeight;
			continue;
		}
		
		if(characters.count(cp) == 0) prerenderCharacter(cp);
		CharacterData characterData = characters[cp];
		if(outline)
			renderGlyphData(characterData.outlineGlyphData, x, y, scale);
		else
			renderGlyphData(characterData.glyphData, x, y, scale);
		
		x += (characterData.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
}