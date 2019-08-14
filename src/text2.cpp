#include "text2.hpp"

#include <utility>
#include <string>
#include <cstddef>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "shaders.hpp"



void TextRenderer2::init() {
	FT_Library ft;
	if(FT_Init_FreeType(&ft))
		throw std::runtime_error("Failed to initialize FreeType");
	
	FT_Face face;
	if(FT_New_Face(ft, "res/NotoSans-Regular.ttf", 0, &face))
		throw std::runtime_error("Failed to load font");
	
	fontHeight = 16;
	FT_Set_Pixel_Sizes(face, 0, fontHeight);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
	
	for(unsigned char c = 0; c < 128; ++c) {
		if(FT_Load_Char(face, c, FT_LOAD_RENDER))
			throw std::runtime_error("Failed to load glyph");
		
		GlId texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows,
			0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		Character character = {
			texture, 
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		characters[c] = character;
	}
	
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	
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
}

void TextRenderer2::setViewport(int width2, int height2) {
	width = width2; height = height2;
}

void TextRenderer2::renderText(std::string str, float startX, float startY, float scale, glm::vec3 color) {
	glUseProgram(program);
	glBindVertexArray(VAO);
	
	glUniform2f(glGetUniformLocation(program, "winSize"), width, height);
	
	glUniform3f(glGetUniformLocation(program, "textColor"), color.r, color.g, color.b);
	
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(program, "tex"), 0);
	
	float x = startX;
	float y = startY;
	float lineHeight = fontHeight * 1.25;
	
	std::string::const_iterator c;
	for(c = str.begin(); c != str.end(); ++c) {
		char c2 = *c;
		
		if(c2 == '\n') {
			x = startX;
			y += lineHeight;
			continue;
		}
		
		if(characters.count(c2) == 0)
			c2 = '?';
		
		Character ch = characters[c2];
		
		float xpos = x + ch.bearing.x * scale;
		float ypos = y - ch.bearing.y * scale;
		
		float w = ch.size.x * scale;
		float h = ch.size.y * scale;
		
		float vertices[4][4] = {            
			{ xpos,     ypos,       0.0, 0.0 },
			{ xpos,     ypos + h,   0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 0.0 },
			{ xpos + w, ypos + h,   1.0, 1.0 }
		};
		
		glBindTexture(GL_TEXTURE_2D, ch.texId);
		
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
}
