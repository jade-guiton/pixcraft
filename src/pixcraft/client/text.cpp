#include "text.hpp"

#include <utility>
#include <string>
#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <array>
#include <algorithm>

using namespace PixCraft;

void TextRenderer::init() {
	if(FT_Init_FreeType(&ft))
		throw std::runtime_error("Failed to initialize FreeType");
	
	FT_Stroker_New(ft, &stroker);
	FT_Stroker_Set(stroker, 64 * 3/2, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
	
	loadFont("res/font/NotoSans-Regular.ttf");
	loadFont("res/font/NotoEmoji-Regular.ttf");
	loadFont("res/font/NotoSansCJKjp-Regular.otf");
	loadFont("res/font/LastResort.ttf");
	
	fontHeight = 16;
	xHeight = 0;
	for(unsigned int i = 0; i < faces.size(); ++i) {
		FT_Set_Pixel_Sizes(faces[i], 0, fontHeight);
	}
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
	
	glyphAtlas.init(512, 512);
	
	// Preload ASCII characters at least
	glyphAtlas.bind();
	for(uint32_t c = 0x20; c < 0x7F; ++c) {
		prerenderCharacter(c);
	}
	
	program.init(ShaderSources::guiVS, ShaderSources::textFS);
	buffer.init(0, 2*sizeof(float), 4*sizeof(float));
	buffer.loadData(nullptr, 6*BUFFER_SIZE, GL_STREAM_DRAW);
	checkGlErrors("text renderer initialization");
}

void TextRenderer::free() {
	for(unsigned int i = 0; i < faces.size(); ++i) {
		FT_Done_Face(faces[i]);
	}
	FT_Done_FreeType(ft);
}

void TextRenderer::setViewport(int width, int height) {
	winWidth = width; winHeight = height;
}

int TextRenderer::getTextWidth(std::string str) {
	std::string::const_iterator it = str.begin();
	std::string::const_iterator end = str.end();
	uint32_t cp;
	
	int width = 0;
	int lineWidth = 0;
	
	while(it != end) {
		cp = utf8::next(it, end);
		if(cp == '\n') {
			lineWidth = 0;
			continue;
		}
		
		if(characters.count(cp) == 0) prerenderCharacter(cp);
		CharacterData characterData = characters[cp];
		
		lineWidth += (characterData.advance >> 6);
		if(lineWidth > width) width = lineWidth;
	}
	
	return width;
}

int TextRenderer::getTextHeight() {
	if(xHeight == 0) {
		if(characters.count('X') == 0) prerenderCharacter('X');
		xHeight = characters['X'].height >> 6;
	}
	return xHeight;
}

void TextRenderer::renderText(std::string str, float startX, float startY, glm::vec4 color) {
	program.use();
	buffer.bind();
	
	glyphAtlas.bind();
	
	program.setUniform("winSize", (float) winWidth, (float) winHeight);
	program.setUniform("tex", (uint32_t) 0);
	program.setUniform("textColor", color.r, color.g, color.b, color.a);
	
	float x = startX;
	float y = startY;
	float lineHeight = round(fontHeight * 1.25);
	
	std::string::const_iterator it = str.begin();
	std::string::const_iterator end = str.end();
	uint32_t cp;
	
	std::array<float, QUAD_SIZE*BUFFER_SIZE> quadBuffer;
	auto bufferIt = quadBuffer.begin();
	
	while(it != end) {
		cp = utf8::next(it, end);
		if(cp == '\n') {
			x = startX;
			y -= lineHeight;
			continue;
		}
		
		if(characters.count(cp) == 0) prerenderCharacter(cp);
		CharacterData characterData = characters[cp];
		
		renderGlyphData(bufferIt, characterData.glyphData, x, y);
		
		if(bufferIt == quadBuffer.end()) {
			bufferIt = quadBuffer.begin();
			buffer.updateData(quadBuffer.data(), quadBuffer.size()/4);
			glDrawArrays(GL_TRIANGLES, 0, quadBuffer.size()/4);
		}
		
		x += characterData.advance >> 6; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	
	if(bufferIt != quadBuffer.begin()) {
		size_t vertices = (bufferIt - quadBuffer.begin())/4;
		buffer.updateData(quadBuffer.data(), vertices);
		glDrawArrays(GL_TRIANGLES, 0, vertices);
	}
	
	buffer.unbind();
	program.unuse();
}

void TextRenderer::loadFont(const char* filename) {
	faces.emplace_back();
	if(FT_New_Face(ft, filename, 0, &faces.back())) {
		std::stringstream errorMsg;
		errorMsg << "Failed to load font " << filename << "." << std::endl;
		throw std::runtime_error(errorMsg.str());
	}
}

void TextRenderer::prerenderCharacter(uint32_t cp) {
	unsigned int i = 0;
	FT_UInt glyphIdx;
	while(i < faces.size() && (glyphIdx = FT_Get_Char_Index(faces[i], cp)) == 0) {
		++i;
	}
	
	if(i == faces.size() || FT_Load_Glyph(faces[i], glyphIdx, FT_LOAD_DEFAULT)) {
		std::stringstream errorMsg;
		errorMsg << "Failed to load glyph for U+" << std::hex << cp << "." << std::endl;
		throw std::runtime_error(errorMsg.str());
	}
	
	FT_GlyphSlot glyphSlot = faces[i]->glyph;
	int32_t advanceX = glyphSlot->advance.x;
	int32_t height = glyphSlot->metrics.height;
	
	FT_Glyph glyph;
	FT_Get_Glyph(glyphSlot, &glyph);
	FT_Glyph outlineGlyph = glyph;
	FT_Glyph_StrokeBorder(&outlineGlyph, stroker, false, false);
	
	FT_Glyph_To_Bitmap(&outlineGlyph, FT_RENDER_MODE_NORMAL, nullptr, true);
	FT_BitmapGlyph outlineBitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(outlineGlyph);
	glm::ivec2 outlineSize(outlineBitmapGlyph->bitmap.width, outlineBitmapGlyph->bitmap.rows);
	
	FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, true);
	FT_BitmapGlyph bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
	glm::ivec2 size(bitmapGlyph->bitmap.width, bitmapGlyph->bitmap.rows);
	glm::ivec2 offset(bitmapGlyph->left - outlineBitmapGlyph->left, outlineBitmapGlyph->top - bitmapGlyph->top);
	
	unsigned char* buffer = new unsigned char[outlineSize.x*outlineSize.y*2];
	for(int x = 0; x < outlineSize.x; ++x) {
		for(int y = 0; y < outlineSize.y; ++y) {
			size_t i = x*2 + y*outlineSize.x*2;
			buffer[i] = outlineBitmapGlyph->bitmap.buffer[x + y*outlineSize.x];
			glm::ivec2 relPos(x - offset.x, y - offset.y);
			if(relPos.x >= 0 && relPos.y >= 0 && relPos.x < size.x && relPos.y < size.y) {
				buffer[i+1] = bitmapGlyph->bitmap.buffer[relPos.x + relPos.y*size.x];
			} else {
				buffer[i+1] = 0;
			}
		}
	}
	unsigned int atlasId = glyphAtlas.addTexture(outlineSize.x, outlineSize.y, buffer);
	
	GlyphData glyphData = GlyphData {
		atlasId,
		outlineSize,
		glm::ivec2(outlineBitmapGlyph->left, outlineBitmapGlyph->top)
	};
	
	characters[cp] = CharacterData {
		glyphData,
		advanceX, height
	};
}

void TextRenderer::renderGlyphData(float*& bufferIt, GlyphData& data, float x, float y) {
	float l = glyphAtlas.getL(data.atlasId);
	float r = glyphAtlas.getR(data.atlasId);
	float t = glyphAtlas.getT(data.atlasId);
	float b = glyphAtlas.getB(data.atlasId);
	
	float w = data.size.x;
	float h = data.size.y;
	float xpos = x + data.bearing.x;
	float ypos = y + data.bearing.y;
	
	float vertices[QUAD_SIZE] = {
		xpos,     ypos,       l, b,
		xpos,     ypos - h,   l, t,
		xpos + w, ypos,       r, b,
		xpos + w, ypos,       r, b,
		xpos,     ypos - h,   l, t,
		xpos + w, ypos - h,   r, t
	};
	std::copy(vertices, vertices + QUAD_SIZE, bufferIt);
	bufferIt += QUAD_SIZE;
}
