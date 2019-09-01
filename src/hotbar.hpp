#pragma once

#include "world/blocks.hpp"
#include "face_renderer.hpp"

namespace PixCraft {
	class Hotbar {
	public:
		Hotbar(FaceRenderer& renderer);
		void init();
		
		void previous();
		void next();
		BlockId held();
		
		void render();
		
	private:
		BlockId _held;
		FaceRenderer& faceRenderer;
		FaceBuffer buffer;
		
		void prerender();
	};
}
