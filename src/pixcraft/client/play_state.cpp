#include "play_state.hpp"

#include <cstdint>
#include <cmath>
#include <array>
#include <sstream>

#include "pixcraft/util/util.hpp"
#include "shaders.hpp"
#include "textures.hpp"
#include "view_frustum.hpp"
#include "menu_state.hpp"

#include "pixcraft/server/blocks.hpp"
#include "pixcraft/server/player.hpp"
#include "pixcraft/server/slime.hpp"

using namespace PixCraft;

const std::array<float, 24> cursorVertices = {
	-1,  -1,   1,  -1,    1,  1,   -1,  1,
	-1, -10,   1, -10,   10, -1,   10,  1,
	 1,  10,  -1,  10,  -10,  1,  -10, -1
};
const std::array<unsigned int, 30> cursorIndices = {
	0,  1,  2,    2,  3,  0,
	0,  4,  5,    5,  1,  0,
	1,  6,  7,    7,  2,  1,
	2,  8,  9,    9,  3,  2,
	3, 10, 11,   11,  0,  3
};

const std::array<float, 8> overlayVertices = {
	1, -1, 1, 1, -1, -1, -1, 1
};

const std::array<float, 24> blockOverlayVertices = {
	-0.505, -0.505, -0.505,
	 0.505, -0.505, -0.505,
	-0.505,  0.505, -0.505,
	-0.505, -0.505,  0.505,
	 0.505,  0.505, -0.505,
	-0.505,  0.505,  0.505,
	 0.505, -0.505,  0.505,
	 0.505,  0.505,  0.505
};
const std::array<unsigned int, 24> blockOverlayIndices = {
	0, 1,  0, 2,  0, 3,
	4, 7,  5, 7,  6, 7,
	1, 4,  1, 6,
	2, 4,  2, 5,
	3, 5,  3, 6
};

PlayState::PlayState(GameClient& client)
	: GameState(client), showDebug(false), paused(false), chunkRenderer(world, faceRenderer),
	  hotbar(faceRenderer) {
	setAntialiasing(false);
	setRenderDistance(8);
	client.getInputManager().capturingMouse(!paused);
	
	cursorProgram.init(ShaderSources::cursorVS, ShaderSources::colorFS);
	cursorBuffer.init(0, 2*sizeof(float));
	cursorBuffer.loadData(cursorVertices.data(), cursorVertices.size()/2, GL_STATIC_DRAW);
	cursorBuffer.loadIndices(cursorIndices.data(), cursorIndices.size());
	checkGlErrors("cursor renderer initialization");
	
	colorOverlayProgram.init(ShaderSources::overlayVS, ShaderSources::colorFS);
	colorOverlayBuffer.init(0, 2*sizeof(float));
	colorOverlayBuffer.loadData(overlayVertices.data(), overlayVertices.size()/2, GL_STATIC_DRAW);
	checkGlErrors("color overlay initialization");
	
	blockOverlayProgram.init(ShaderSources::blockOverlayVS, ShaderSources::colorFS);
	blockOverlayBuffer.init(0, 3*sizeof(float));
	blockOverlayBuffer.loadData(blockOverlayVertices.data(), blockOverlayVertices.size()/3, GL_STATIC_DRAW);
	blockOverlayBuffer.loadIndices(blockOverlayIndices.data(), blockOverlayIndices.size());
	checkGlErrors("block overlay initialization");
	
	faceRenderer.init();
	entityRenderer.init();
	particleRenderer.init();
	hotbar.init();
	
	console.addCommand("clear", [&]() {
		console.clearHistory();
	});
	console.addCommand("fly", [&]() {
		player->movementMode(MovementMode::flying);
	});
	console.addCommand("fall", [&]() {
		player->movementMode(MovementMode::normal);
	});
	console.addCommand("noclip", [&]() {
		player->movementMode(MovementMode::noClip);
	});
	console.addCommand("debug", [&]() {
		showDebug = !showDebug;
	});
	console.addCommand("antialias", [&]() {
		setAntialiasing(!antialiasing);
	});
	console.addCommand("further", [&]() {
		setRenderDistance(renderDist + 1);
	});
	console.addCommand("closer", [&]() {
		if(renderDist > 1)
			setRenderDistance(renderDist - 1);
	});
	console.addCommand("rerender", [&]() {
		chunkRenderer.reset();
	});
	console.addCommand("save", [&]() {
		world.saveToFile("data/world.bin");
		console.write("Saved world to file.");
	});
	console.addCommand("load", [&]() {
		world.loadFromFile("data/world.bin");
		chunkRenderer.reset();
		console.write("Loaded world from file.");
	});
	
	menuButtons.emplace_back(0, 0, 200, 30, "Back to menu");
	menuButtons.back().setCallback([&client]() {
		client.setGameState(new MenuState(client));
	});
	
	for(auto& button : menuButtons) {
		button.prerender();
	}
	
	world.mobs.emplace_back(new Player(world, glm::vec3(8.0f, 50.0f, 8.0f)));
	player = (Player*) world.mobs.back().get();
	world.mobs.emplace_back(new Slime(world, glm::vec3(0.0f, 50.0f, 0.0f)));
}

void PlayState::setAntialiasing(bool enabled) {
	if(enabled) {
		glEnable(GL_MULTISAMPLE);
	} else {
		glDisable(GL_MULTISAMPLE);
	}
	antialiasing = enabled;
}

void PlayState::setRenderDistance(int renderDist2) {
	renderDist = renderDist2;
	fogEnd = renderDist * CHUNK_SIZE;
	fogStart = fogEnd * 0.9;
}

void PlayState::update(float dt) {
	InputManager& input = client.getInputManager();
	
	console.update(input);
	if(!console.isOpen()) {
		if(input.justPressed(GLFW_KEY_ESCAPE)) {
			paused = !paused;
			client.getInputManager().capturingMouse(!paused);
		}
		
		if(paused) {
			if(input.justClicked(1)) {
				glm::ivec2 pos = input.getMousePosition();
				Button* hit = nullptr;
				for(auto& button : menuButtons) {
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
	}
	
	if(!paused && !console.isOpen()) {
		player->handleKeys(input.getMovementKeys(), dt);
		
		glm::vec2 mouseMvt = input.getMouseMovement();
		player->rotate(glm::vec3(-mouseMvt.y, -mouseMvt.x, 0));
		
		int scroll = input.justScrolled();
		if(scroll > 0) {
			for(int i = 0; i < scroll; i++) {
				hotbar.next();
			}
		} else if(scroll < 0) {
			for(int i = 0; i < -scroll; i++) {
				hotbar.previous();
			}
		}
		
		bool click1 = input.justClicked(1);
		bool click2 = input.justClicked(2);
		if(click1 || click2) {
			bool hit;
			int x, y, z;
			std::tie(hit, x, y, z) = player->castRay(PLAYER_REACH, !click1, false);
			if(hit && World::isValidHeight(y)) {
				if(click2) {
					if(!world.hasSolidBlock(x, y, z) && !world.containsMobs(x, y, z))
						world.setBlock(x, y, z, Block::fromId(hotbar.held()));
				} else {
					auto blockTex = world.getBlock(x, y, z)->mainTexture();
					world.removeBlock(x, y, z);
					particleRenderer.spawnBlockBits(glm::vec3((float) x, (float) y, (float) z), blockTex);
				}
			}
		}
	} else {
		input.getMouseMovement();
		player->handleKeys(std::tuple<int,int,bool,bool>(0,0,false,false), dt);
	}
	
	int32_t camX, camY, camZ;
	std::tie(camX, camY, camZ) = getBlockCoordsAt(player->pos());
	int32_t camChunkX, camChunkZ;
	std::tie(camChunkX, camChunkZ) = World::getChunkPosAt(camX, camZ);
	int loads = 0;
	SpiralIterator iter(camChunkX, camChunkZ);
	while(iter.withinSquareDistance(renderDist + 1)) {
		if(iter.withinDistance(renderDist + 2)) {
			int x = iter.getX();
			int z = iter.getZ();
			if(!world.isChunkLoaded(x, z)) {
				world.genChunk(x, z);
				loads++;
			} else if(!chunkRenderer.isChunkRendered(x, z)) {
				world.markChunkDirty(x, z);
				loads++;
			}
			if(loads >= LOADS_PER_FRAME) break;
		}
		iter.next();
	}
	
	world.updateBlocks();
	chunkRenderer.updateBlocks();
	
	world.updateEntities(dt);
	
	particleRenderer.update(dt);
}

void printElapsedTime(const char* operation, double before) {
	std::cout << operation << ": " << round((glfwGetTime() - before)*10000) / 10.0 << " ms" << std::endl;
}

void PlayState::render(int winWidth, int winHeight) {
	// Compute some rendering data based on player position
	float fovy = glm::radians(90.0f);
	float aspect = ((float) winWidth) / winHeight;
	float near = 0.001f;
	float far = 1000.0f;
	glm::vec3 playerPos = player->eyePos();
	glm::mat4 proj = glm::perspective(fovy, aspect, near, far);
	glm::mat4 view = globalToLocal(playerPos, player->orient());
	
	ViewFrustum vf = computeViewFrustum(fovy, aspect, near, far, playerPos, player->orient());
	
	int32_t camX, camY, camZ;
	std::tie(camX, camY, camZ) = getBlockCoordsAt(player->pos());
	int32_t camChunkX, camChunkZ;
	std::tie(camChunkX, camChunkZ) = World::getChunkPosAt(camX, camZ);
	
	// Clear screen
	glClearColor(SKY_COLOR[0], SKY_COLOR[1], SKY_COLOR[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// Start the face rendering
	RenderParams params = {
		{ SKY_COLOR[0], SKY_COLOR[1], SKY_COLOR[2] },
		true, true,
		fogStart, fogEnd,
	};
	faceRenderer.startRendering(proj, view, params);
	chunkRenderer.render(camChunkX, camChunkZ, renderDist, vf);
	faceRenderer.stopRendering();
	checkGlErrors("block rendering");
	
	entityRenderer.renderEntities(world, proj, view, params);
	checkGlErrors("entity rendering");
	
	bool hit;
	int x, y, z;
	std::tie(hit, x, y, z) = player->castRay(PLAYER_REACH, false, false);
	if(hit) {
		blockOverlayProgram.use();
		blockOverlayProgram.setUniform("view", view);
		blockOverlayProgram.setUniform("proj", proj);
		glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3((float) x, (float) y, (float) z));
		blockOverlayProgram.setUniform("model", model);
		blockOverlayProgram.setUniform("color", 0.0f, 0.0f, 0.0f, 1.0f);
		blockOverlayBuffer.bind();
		glDrawElements(GL_LINES, sizeof(blockOverlayIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
		blockOverlayBuffer.unbind();
		blockOverlayProgram.unuse();
		checkGlErrors("block overlay rendering");
	}
	
	particleRenderer.render(proj, view, fovy, winHeight);
	checkGlErrors("particle rendering");
	
	faceRenderer.startRendering(proj, view, params);
	chunkRenderer.renderTranslucent(camChunkX, camChunkZ, renderDist, vf);
	checkGlErrors("translucent block rendering");
	
	glClear(GL_DEPTH_BUFFER_BIT);
	params.applyView = false;
	params.applyFog = false;
	faceRenderer.setParams(params);
	hotbar.render();
	faceRenderer.stopRendering();
	checkGlErrors("held block rendering");
	
	// After this point, no depth testing needed
	glDisable(GL_DEPTH_TEST);
	
	// Draw underwater overlay
	if(player->isEyeUnderwater()) {
		colorOverlayProgram.use();
		colorOverlayProgram.setUniform("color", 0.11f, 0.43f, 0.97f, 0.3f);
		colorOverlayBuffer.bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		colorOverlayBuffer.unbind();
		colorOverlayProgram.unuse();
		checkGlErrors("water overlay rendering");
	}
	
	// Draw cursor
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	cursorProgram.use();
	cursorProgram.setUniform("winSize", (float) winWidth, (float) winHeight);
	cursorProgram.setUniform("color", 1.0f, 1.0f, 1.0f, 1.0f);
	cursorBuffer.bind();
	glDrawElements(GL_TRIANGLES, cursorBuffer.indexCount(), GL_UNSIGNED_INT, 0);
	cursorBuffer.unbind();
	cursorProgram.unuse();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	checkGlErrors("cursor rendering");
	
	TextRenderer& textRenderer = client.getTextRenderer();
	
	if(paused) {
		colorOverlayProgram.use();
		colorOverlayProgram.setUniform("color", 0.0f, 0.0f, 0.0f, 0.5f);
		colorOverlayBuffer.bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		colorOverlayBuffer.unbind();
		colorOverlayProgram.unuse();
		checkGlErrors("pause menu overlay rendering");
		
		for(auto& button : menuButtons) {
			button.render(textRenderer, winWidth, winHeight);
		}
		checkGlErrors("button rendering");
	}
	
	if(showDebug) {
		// Draw debug data
		std::stringstream debugStream;
		debugStream << client.getFPS() << " FPS" << std::endl;
		debugStream << "Pos: " << vec3ToString(playerPos) << std::endl;
		debugStream << "Mode: " << movementModeNames[static_cast<int>(player->movementMode())] << std::endl;
		debugStream << "Vertical speed: " << player->speed().y << std::endl;
		debugStream << "Rendered chunks: " << chunkRenderer.renderedChunkCount() << std::endl;
		debugStream << "Antialiasing: " << (antialiasing ? "enabled" : "disabled") << std::endl;
		//debugStream << "Unicode test: AéǄ‰₪ℝψЯאصखଇணఔฌ갃ば亶〠㊆😎😂" << std::endl;
		textRenderer.renderText(debugStream.str(), -winWidth/2 + 5, winHeight/2 - 20, glm::vec3(1.0, 1.0, 1.0));
		checkGlErrors("debug text rendering");
	}
	
	console.render(textRenderer, winWidth, winHeight);
}