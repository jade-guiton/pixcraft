#include "menu_state.hpp"

using namespace PixCraft;

MenuState::MenuState(GameClient& client)
	: GameState(client), logo(0, 100, 100, TextureManager::LOGO), playButton(0, 0, 300, 30, "Play") {
	client.getInputManager().capturingMouse(false);
	
	Image::initRendering();
	
	logo.prerender();
	playButton.prerender();
}

void MenuState::update(float dt) {
	
}

void MenuState::render(int winWidth, int winHeight) {
	TextRenderer& textRenderer = client.getTextRenderer();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	
	logo.render(winWidth, winHeight);
	checkGlErrors("logo rendering");
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	playButton.render(textRenderer, winWidth, winHeight);
	checkGlErrors("play button rendering");
}
