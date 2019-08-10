#define GLT_IMPLEMENTATION
#include "text.hpp"

#include <stdexcept>

Text::Text(std::string str, float x, float y, float scale, glm::vec4 color)
	: x(x), y(y), scale(scale), color(color) {
	text = gltCreateText();
	gltSetText(text, str.c_str());
}

void Text::deleteText() {
	gltDeleteText(text);
}

void Text::render() {
	gltColor(color.r, color.g, color.b, color.a);
	gltDrawText2D(text, x, y, scale);
}

void Text::setText(std::string str) {
	gltSetText(text, str.c_str());
}


void TextRenderer::init() {
	if(!gltInit()) {
		throw std::runtime_error("Failed to initialize GLText");
	}
}

void TextRenderer::terminate() {
	for(auto& text : texts) {
		text.deleteText();
	}
	gltTerminate();
}

void TextRenderer::setViewport(int width, int height) {
	gltViewport(width, height);
}

Text& TextRenderer::createText(std::string str, float x, float y, float scale, glm::vec4 color) {
	texts.emplace_back(str, x, y, scale, color);
	return texts.back();
}

void TextRenderer::render() {
	gltBeginDraw();
	for(auto& text : texts) {
		text.render();
	}
	gltEndDraw();
}
