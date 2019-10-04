#include "gui.hpp"

#include <iostream>

#include "textures.hpp"
#include "glfw.hpp"

using namespace PixCraft;

ShaderProgram Image::program;

void Image::initRendering() {
	program.init(ShaderSources::guiVS, ShaderSources::textureFS);
}

Image::Image(int x, int y, int w, TexId tex) : x(x), y(y), w(w), tex(tex) {
	h = TextureManager::getTextureHeight(tex) * w / TextureManager::getTextureWidth(tex);
}

void Image::prerender() {
	const std::array<float, 16> imageVertices {
		x-w/2, y-h/2, 0, 0,  x+w/2, y-h/2, 1, 0,
		x-w/2, y+h/2, 0, 1,  x+w/2, y+h/2, 1, 1,
	};
	buffer.init(0, 2*sizeof(float), 4*sizeof(float));
	buffer.loadData(imageVertices.data(), imageVertices.size()/4, GL_STATIC_DRAW);
	checkGlErrors("image prerendering");
}

void Image::render(int winW, int winH) {
	program.use();
	checkGlErrors("program activation");
	program.setUniform("winSize", (float) winW, (float) winH);
	program.setUniform("tex", (uint32_t) 0);
	checkGlErrors("uniform setting");
	TextureManager::bindOtherTextures(tex);
	buffer.bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, buffer.vertexCount());
	buffer.unbind();
	program.unuse();
}

const std::array<unsigned int, 54> buttonIndices = {
	0, 1, 2,     2, 1, 3,
	1, 5, 3,     3, 5, 7,
	5, 4, 7,     7, 4, 6,
	7, 6, 15,    15, 6, 14,
	15, 14, 13,  13, 14, 12,
	11, 15, 9,   9, 15, 13,
	10, 11, 8,   8, 11, 9,
	2, 3, 10,    10, 3, 11,
	3, 7, 11,    11, 7, 15
};

ShaderProgram Button::program;
float Button::cs;

void Button::initRendering() {
	program.init(ShaderSources::guiVS, ShaderSources::textureFS);
	cs = TextureManager::getTextureWidth(TextureManager::BUTTON) / 2;
}

Button::Button(int x, int y, int w, int h, std::string label)
	: x(x), y(y), w(w), h(h), label(label) {
}

void Button::prerender() {
	const std::array<float, 64> buttonVertices {
		x-w/2, y-h/2-sh, 0, 0,  x-w/2+cs, y-h/2-sh, 0.5, 0,  x-w/2, y-h/2-sh+cs, 0, 0.5,  x-w/2+cs, y-h/2-sh+cs, 0.5, 0.5,
		x+w/2, y-h/2-sh, 1, 0,  x+w/2-cs, y-h/2-sh, 0.5, 0,  x+w/2, y-h/2-sh+cs, 1, 0.5,  x+w/2-cs, y-h/2-sh+cs, 0.5, 0.5,
		x-w/2, y+h/2,    0, 1,  x-w/2+cs, y+h/2,    0.5, 1,  x-w/2, y+h/2-cs,    0, 0.5,  x-w/2+cs, y+h/2-cs,    0.5, 0.5,
		x+w/2, y+h/2,    1, 1,  x+w/2-cs, y+h/2,    0.5, 1,  x+w/2, y+h/2-cs,    1, 0.5,  x+w/2-cs, y+h/2-cs,    0.5, 0.5,
	};
	buffer.init(0, 2*sizeof(float), 4*sizeof(float));
	buffer.loadData(buttonVertices.data(), buttonVertices.size()/4, GL_STATIC_DRAW);
	buffer.loadIndices(buttonIndices.data(), buttonIndices.size());
	checkGlErrors("button prerendering");
}

void Button::render(TextRenderer& textRenderer, int winW, int winH) {
	program.use();
	program.setUniform("winSize", (float) winW, (float) winH);
	program.setUniform("tex", (uint32_t) 0);
	TextureManager::bindOtherTextures(TextureManager::BUTTON);
	buffer.bind();
	glDrawElements(GL_TRIANGLES, buffer.indexCount(), GL_UNSIGNED_INT, 0);
	buffer.unbind();
	program.unuse();
	int textX = winW/2 + x - textRenderer.getTextWidth(label)/2;
	int textY = winH/2 + y - textRenderer.getTextHeight()/2;
	textRenderer.renderText(label, textX, textY, glm::vec3(0.0, 0.0, 0.0));
}

bool Button::hits(int hx, int hy) {
	return x - w/2 <= hx && hx < x + w/2 && y - h/2 <= hy && hy < y + h/2 ;
}

void Button::click() {
	std::cout << "Just clicked on button '" << label << "'" << std::endl;
}
