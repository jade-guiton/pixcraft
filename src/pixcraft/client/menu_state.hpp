#pragma once

#include "client.hpp"
#include "gui.hpp"

namespace PixCraft {
	class MenuState : public GameState {
	public:
		MenuState(GameClient& client);
		
		void update(float dt) override;
		void render(int winWidth, int winHeight) override;
		
	private:
		Image logo;
		std::vector<Button> buttons;
	};
}
