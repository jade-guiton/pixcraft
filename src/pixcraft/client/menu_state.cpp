#include "menu_state.hpp"

using namespace PixCraft;

MenuState::MenuState(GameClient& client)
	: GameState(client), playButton(0, 0, 300, 30, "Play") {
	client.getInputManager().capturingMouse(false);
	
	playButton.prerender();
}

void MenuState::update(float dt) {
	
}

void MenuState::render(int winWidth, int winHeight) {
	TextRenderer& textRenderer = client.getTextRenderer();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	playButton.render(textRenderer, winWidth, winHeight);
	checkGlErrors("play button rendering");
}
