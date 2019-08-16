#include "text.hpp"

#include <utility>
#include <string>
#include <cstddef>

#include "shaders.hpp"


void TextRenderer::init() {
	if(FT_Init_FreeType(&ft))
		throw std::runtime_error("Failed to initialize FreeType");
	
	if(FT_New_Face(ft, "res/NotoSans-Regular.ttf", 0, &face))
		throw std::runtime_error("Failed to load font");
	
	fontHeight = 16;
	FT_Set_Pixel_Sizes(face, 0, fontHeight);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
	
	glyphAtlas.init(512, 512);
	
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

TextRenderer::~TextRenderer() {
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void TextRenderer::setViewport(int width, int height) {
	winWidth = width; winHeight = height;
}

void TextRenderer::renderText(std::string str, float startX, float startY, float scale, glm::vec3 color) {
	glUseProgram(program);
	glBindVertexArray(VAO);
	
	glyphAtlas.bind();
	
	glUniform2f(glGetUniformLocation(program, "winSize"), winWidth, winHeight);
	glUniform3f(glGetUniformLocation(program, "textColor"), color.r, color.g, color.b);
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
			prerenderCharacter(c2);
		
		Glyph ch = characters[c2];
		
		float xpos = x + ch.bearing.x * scale;
		float ypos = y - ch.bearing.y * scale;
		
		float l = glyphAtlas.getL(ch.atlasId);
		float r = glyphAtlas.getR(ch.atlasId);
		float t = glyphAtlas.getT(ch.atlasId);
		float b = glyphAtlas.getB(ch.atlasId);
		
		float w = ch.size.x * scale;
		float h = ch.size.y * scale;
		
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
		
		x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
}

void TextRenderer::prerenderCharacter(char c) {
	if(FT_Load_Char(face, c, FT_LOAD_RENDER))
		throw std::runtime_error("Failed to load glyph");
	
	glm::ivec2 size(face->glyph->bitmap.width, face->glyph->bitmap.rows);
	
	unsigned int atlasId = glyphAtlas.addTexture(size.x, size.y, face->glyph->bitmap.buffer);
	Glyph character = {
		atlasId,
		size,
		glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
		face->glyph->advance.x
	};
	characters[c] = character;
}