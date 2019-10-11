#include "menu_state.hpp"

#include <iostream>
#include <memory>

#include "play_state.hpp"

using namespace PixCraft;

MenuState::MenuState(GameClient& client)
	: GameState(client), logo(0, 60, 100, TextureManager::LOGO) {
	client.getInputManager().capturingMouse(false);
	
	buttons.emplace_back(0, -50, 300, 30, "Play");
	buttons.back().setCallback([&client]() {
		client.setGameState(new PlayState(client));
	});
	buttons.emplace_back(0, -90, 300, 30, "Exit");
	buttons.back().setCallback([&client]() {
		client.stop();
	});
	
	bgProgram.init(ShaderSources::menuBgVS, ShaderSources::menuBgFS);
	Image::initRendering();
	
	const std::array<float, 8> bgVertices {
		-1, -1,  1, -1,
		-1,  1,  1,  1,
	};
	bgBuffer.init(0, 2*sizeof(float));
	bgBuffer.loadData(bgVertices.data(), bgVertices.size()/2, GL_STATIC_DRAW);
	
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
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	TextRenderer& textRenderer = client.getTextRenderer();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	
	bgProgram.use();
	bgProgram.setUniform("texArray", (uint32_t) 0);
	TextureManager::bindBlockTextureArray();
	bgProgram.setUniform("texId", (uint32_t) TextureManager::DIRT);
	bgProgram.setUniform("winSize", (float) winWidth, (float) winHeight);
	bgProgram.setUniform("tileSize", (float) TextureManager::BLOCK_TEX_SIZE*4);
	float t = client.getFrameNo() / 60.0f;
	bgProgram.setUniform("offset", t * 0.5f, -t * 0.3f);
	bgBuffer.bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, bgBuffer.vertexCount());
	bgBuffer.unbind();
	bgProgram.unuse();
	checkGlErrors("menu background rendering");
	
	logo.render(winWidth, winHeight);
	checkGlErrors("logo rendering");
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	for(auto& button : buttons) {
		button.render(textRenderer, winWidth, winHeight);
	}
	checkGlErrors("button rendering");
}
