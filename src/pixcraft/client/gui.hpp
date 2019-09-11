#pragma once

#include "shaders.hpp"
#include "text.hpp"

namespace PixCraft {
	class Button {
	public:
		static void initRendering();
		
		Button(int x, int y, int w, int h, std::string label);
		void prerender();
		void render(TextRenderer& textRenderer, int winW, int winH);
		
	private:
		static ShaderProgram program;
		static constexpr float sh = 6; // shadow height
		static float cs; // corner size
		
		float x, y, w, h;
		std::string label;
		IndexBuffer<glm::vec2, glm::vec2> buffer;
	};
}