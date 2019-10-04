#include "menu_state.hpp"

#include <iostream>
#include <memory>

#include "play_state.hpp"

using namespace PixCraft;

MenuState::MenuState(GameClient& client)
	: GameState(client), logo(0, 60, 100, TextureManager::LOGO) {
	client.getInputManager().capturingMouse(false);
	
	buttons.emplace_back(0, -40, 300, 30, "Play");
	buttons.back().setCallback([&client]() {
		std::cout << "Let's play!" << std::endl;
		client.setGameState(std::unique_ptr<PlayState>(new PlayState(client)));
	});
	
	Image::initRendering();
	
	logo.prerender();
	for(auto& button : buttons) {
		button.prerender();
	}
}

void MenuState::update(float dt) {
	InputManager& input = client.getInputManager();
	if(input.justClicked(1)) {
		glm::ivec2 pos = input.getMousePosition();
		Button* hit = nullptr;
		for(auto& button : buttons) {
			if(button.hits(pos.x, pos.y)) {
				hit = &button;
				break;
			}
		}
		if(hit) {
			hit->click();
		}
	}
}

void MenuState::render(int winWidth, int winHeight) {
	TextRenderer& textRenderer = client.getTextRenderer();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	
	logo.render(winWidth, winHeight);
	checkGlErrors("logo rendering");
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	for(auto& button : buttons) {
		button.render(textRenderer, winWidth, winHeight);
	}
	checkGlErrors("button rendering");
}
