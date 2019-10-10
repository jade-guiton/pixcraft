#pragma once

#include "client.hpp"
#include "gui.hpp"
#include "shaders.hpp"

namespace PixCraft {
	class MenuState : public GameState {
	public:
		MenuState(GameClient& client);
		
		void update(float dt) override;
		void render(int winWidth, int winHeight) override;
		
	private:
		ShaderProgram bgProgram;
		VertexBuffer<glm::vec2> bgBuffer;
		
		Image logo;
		std::vector<Button> buttons;
	};
}
