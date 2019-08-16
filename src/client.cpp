#include "client.hpp"

#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <cmath>
#include <sstream>

#include "glm.hpp"

#include "util.hpp"
#include "shaders.hpp"
#include "textures.hpp"
#include "view_frustum.hpp"

#include "world/blocks.hpp"
#include "world/player.hpp"
#include "world/slime.hpp"

const char windowTitle[] = "OpenGL Test 2";

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	std::cout << "Window resized to " << width << "×" << height << std::endl;
	
	GameClient& client = *((GameClient*) glfwGetWindowUserPointer(window));
	client.getTextRenderer().setViewport(width, height);
}

const float cursorVertices[] = {
	-1,  -1,   1,  -1,    1,  1,   -1,  1,
	-1, -10,   1, -10,   10, -1,   10,  1,
	 1,  10,  -1,  10,  -10,  1,  -10, -1
};
const size_t cursorIndexCount = 30;
const unsigned int cursorIndices[] = {
	0,  1,  2,    2,  3,  0,
	0,  4,  5,    5,  1,  0,
	1,  6,  7,    7,  2,  1,
	2,  8,  9,    9,  3,  2,
	3, 10, 11,   11,  0,  3
};

const float overlayVertices[] = {
	1, -1, 1, 1, -1, -1, -1, 1
};

GameClient::GameClient()
	: showDebug(false), chunkRenderer(world, faceRenderer, RENDER_DIST), hotbar(faceRenderer), paused(false),
	  frameNo(0), FPS(0.0) {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	
	window = glfwCreateWindow(START_WIDTH, START_HEIGHT, windowTitle, nullptr, nullptr);
	if(window == nullptr)
		throw std::runtime_error("Failed to create GLFW window");
	glfwSetWindowUserPointer(window, (void*) this);
	
	glfwMakeContextCurrent(window);
	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
		throw std::runtime_error("Failed to initialize GLAD");
	
	std::cout << "Using OpenGL version " << GLVersion.major << "." << GLVersion.minor << std::endl;
	
	setAntialiasing(false);
	
	glfwSetFramebufferSizeCallback(window, windowResizeCallback);
	windowResizeCallback(window, START_WIDTH, START_HEIGHT);
	glfwSwapInterval(-1);
	
	cursorProgram = loadCursorProgram();
	glGenVertexArrays(1, &cursorVAO);
	GlId cursorVBO, cursorEBO;
	glGenBuffers(1, &cursorVBO);
	glGenBuffers(1, &cursorEBO);
	
	glBindVertexArray(cursorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cursorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cursorVertices), cursorVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cursorEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cursorIndices), cursorIndices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	checkGlErrors("cursor renderer initialization");
	
	colorOverlayProgram = loadColorOverlayProgram();
	glGenVertexArrays(1, &colorOverlayVAO);
	GlId colorOverlayVBO;
	glGenBuffers(1, &colorOverlayVBO);
	
	glBindVertexArray(colorOverlayVAO);
	glBindBuffer(GL_ARRAY_BUFFER, colorOverlayVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(overlayVertices), overlayVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	checkGlErrors("color overlay initialization");
	
	TextureManager::loadTextures();
	BlockRegistry::defineBlocks();
	faceRenderer.init();
	textRenderer.init();
	entityRenderer.init();
	hotbar.init();
	
	world.mobs.emplace_back(new Player(world, glm::vec3(8.0f, 50.0f, 8.0f)));
	player = (Player*) world.mobs.back().get();
	world.mobs.emplace_back(new Slime(world, glm::vec3(0.0f, 50.0f, 0.0f)));
	
	input.init(window);
	input.capturingMouse(!paused);
	
	glfwSetTime(0.0);
}

GameClient::~GameClient() {
	glfwDestroyWindow(window);
}

constexpr float GameClient::SKY_COLOR[];

void GameClient::mainLoop() {
	glfwSetTime(0.0);
	
	int frameCounter = 0;
	
	float dt = 1 / 60.0;
	double now = glfwGetTime();
	double lastFrame = now;
	double lastSecond = now;
	while(!glfwWindowShouldClose(window)) {
		update(dt);
		render();
		
		now = glfwGetTime();
		if(frameNo != 0) {
			dt = now - lastFrame;
			if(dt > 1 / 30.0) {
				std::cout << "Can't keep up!" << std::endl;
				dt = 1/60.0;
			}
		}
		++frameNo;
		lastFrame = now;
		
		frameCounter++;
		if(now >= lastSecond + 1.0) {
			FPS = frameCounter;
			frameCounter = 0;
			if(now >= lastSecond + 2.0) {
				lastSecond = now;
			} else {
				lastSecond += 1.0;
			}
		}
		
		glfwSwapBuffers(window);
	}
}

InputManager& GameClient::getInputManager() { return input; }
TextRenderer& GameClient::getTextRenderer() { return textRenderer; }

void GameClient::setAntialiasing(bool enabled) {
	if(enabled) {
		glEnable(GL_MULTISAMPLE);
	} else {
		glDisable(GL_MULTISAMPLE);
	}
	antialiasing = enabled;
}

void GameClient::update(float dt) {
	glfwPollEvents();
	
	if(!paused) {
		if(input.justPressed(GLFW_KEY_F)) {
			if(player->movementMode() == MovementMode::flying) {
				player->movementMode(MovementMode::normal);
			} else {
				player->movementMode(MovementMode::flying);
			}
		}
		if(input.justPressed(GLFW_KEY_N)) {
			if(player->movementMode() == MovementMode::noClip) {
				player->movementMode(MovementMode::normal);
			} else {
				player->movementMode(MovementMode::noClip);
			}
		}
		
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
			std::tie(hit, x, y, z) = player->castRay(5, !click1, false);
			if(hit && World::isValidHeight(y)) {
				if(click2) {
					if(!world.hasSolidBlock(x, y, z) && !world.containsMobs(x, y, z))
						world.setBlock(x, y, z, Block::fromId(hotbar.held()));
				} else {
					world.removeBlock(x, y, z);
				}
			}
		}
	} else {
		player->handleKeys(std::tuple<int,int,bool,bool>(0,0,false,false), dt);
	}
	input.clearJustClicked();
	input.clearJustScrolled();
	
	if(input.justPressed(GLFW_KEY_ESCAPE)) {
		paused = !paused;
		input.capturingMouse(!paused);
	}
	if(input.justPressed(GLFW_KEY_G)) {
		showDebug = !showDebug;
	}
	if(input.justPressed(GLFW_KEY_H)) {
		setAntialiasing(!antialiasing);
	}
	input.clearJustPressed();
	
	world.updateBlocks();
	chunkRenderer.updateBlocks();
	
	world.updateEntities(dt);
	
	int32_t camChunkX = floor(player->pos().x / CHUNK_SIZE);
	int32_t camChunkZ = floor(player->pos().z / CHUNK_SIZE);
	int loads = 0;
	SpiralIterator iter(camChunkX, camChunkZ);
	while(iter.withinDistance(RENDER_DIST)) {
		int x = iter.getX();
		int z = iter.getZ();
		if(!world.isChunkLoaded(x, z)) {
			world.genChunk(x, z);
			loads++;
		}
		if(!chunkRenderer.isChunkRendered(x, z)) {
			chunkRenderer.prerenderChunk(x, z);
			loads++;
		}
		if(loads >= LOADS_PER_FRAME) break;
		iter.next();
	}
}

void GameClient::render() {
	// Compute some rendering data based on player position
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	float fovy = glm::radians(90.0f);
	float aspect = ((float) width) / height;
	float near = 0.001f;
	float far = 1000.0f;
	glm::vec3 playerPos = player->eyePos();
	glm::mat4 proj = glm::perspective(fovy, aspect, near, far);
	glm::mat4 view = globalToLocal(playerPos, player->orient());
	
	ViewFrustum vf = computeViewFrustum(fovy, aspect, near, far, playerPos, player->orient());
	
	int32_t camChunkX = floor(playerPos.x / CHUNK_SIZE);
	int32_t camChunkZ = floor(playerPos.z / CHUNK_SIZE);
	
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
		FOG_START, FOG_END,
	};
	faceRenderer.startRendering(proj, view, params);
	chunkRenderer.render(camChunkX, camChunkZ, vf);
	faceRenderer.stopRendering();
	checkGlErrors("block rendering");
	
	entityRenderer.renderEntities(world, proj, view, params);
	checkGlErrors("entity rendering");
	
	faceRenderer.startRendering(proj, view, params);
	chunkRenderer.renderTranslucent(camChunkX, camChunkZ, vf);
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
		glUseProgram(colorOverlayProgram);
		glUniform4f(glGetUniformLocation(colorOverlayProgram, "color"), 0.11f, 0.43f, 0.97f, 0.3f);
		glBindVertexArray(colorOverlayVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		glUseProgram(0);
		checkGlErrors("water overlay rendering");
	}
	
	// Draw cursor
	glUseProgram(cursorProgram);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glUniform2f(glGetUniformLocation(cursorProgram, "winSize"), width, height);
	glUniform4f(glGetUniformLocation(cursorProgram, "color"), 1, 1, 1, 1);
	glBindVertexArray(cursorVAO);
	glDrawElements(GL_TRIANGLES, cursorIndexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUseProgram(0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	checkGlErrors("cursor rendering");
	
	if(paused) {
		glUseProgram(colorOverlayProgram);
		glUniform4f(glGetUniformLocation(colorOverlayProgram, "color"), 0.0f, 0.0f, 0.0f, 0.5f);
		glBindVertexArray(colorOverlayVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		glUseProgram(0);
		checkGlErrors("pause menu overlay rendering");
	}
	
	if(showDebug) {
		// Draw debug data
		std::stringstream debugStream;
		debugStream << FPS << " FPS" << std::endl;
		debugStream << "Pos: " << vec3ToString(playerPos) << std::endl;
		debugStream << "Mode: " << movementModeNames[static_cast<int>(player->movementMode())] << std::endl;
		debugStream << "Vertical speed: " << player->speed().y << std::endl;
		debugStream << "Rendered chunks: " << chunkRenderer.renderedChunkCount() << std::endl;
		debugStream << "Antialiasing: " << (antialiasing ? "enabled" : "disabled") << std::endl;
		textRenderer.renderText(debugStream.str(), 5, 20, 1.0, glm::vec3(1.0, 1.0, 1.0));
		checkGlErrors("debug text rendering");
	}
}
