#pragma once

#include <functional>

#include "shaders.hpp"
#include "textures.hpp"
#include "text.hpp"

namespace PixCraft {
	class Image {
	public:
		static void initRendering();
		
		Image(int x, int y, int w, TexId tex);
		void prerender();
		void render(int winW, int winH);
		
	private:
		static ShaderProgram program;
		
		float x, y, w, h;
		TexId tex;
		VertexBuffer<glm::vec2, glm::vec2> buffer;
	};
	
	class Button {
	public:
		static void initRendering();
		
		Button(int x, int y, int w, int h, std::string label);
		void prerender();
		void render(TextRenderer& textRenderer, int winW, int winH);
		
		bool hits(int x, int y);
		void setCallback(std::function<void()> callback);
		void click();
		
	private:
		static ShaderProgram program;
		static constexpr float sh = 6; // shadow height
		static float cs; // corner size
		
		float x, y, w, h;
		std::string label;
		IndexBuffer<glm::vec2, glm::vec2> buffer;
		
		std::function<void()> callback;
	};
}