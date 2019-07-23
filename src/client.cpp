#include "client.hpp"

const char windowTitle[] = "OpenGL Test 2";

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	std::cout << "Window resized to " << width << "×" << height << std::endl;
	
	GameClient& client = *((GameClient*) glfwGetWindowUserPointer(window));
	client.getTextRenderer().setViewport(width, height);
}

const float cursorVertices[] = {
	-10, 0, 10, 0, 0, -10, 0.0, 10
};

GameClient::GameClient()
	: blockRenderer(RENDER_DIST), paused(false), firstFrame(true), FPS(0.0) {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	window = glfwCreateWindow(START_WIDTH, START_HEIGHT, windowTitle, nullptr, nullptr);
	if(window == nullptr)
		throw std::runtime_error("Failed to create GLFW window");
	glfwSetWindowUserPointer(window, (void*) this);
	
	glfwMakeContextCurrent(window);
	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
		throw std::runtime_error("Failed to initialize GLAD");
	
	glfwSetFramebufferSizeCallback(window, windowResizeCallback);
	windowResizeCallback(window, START_WIDTH, START_HEIGHT);
	glfwSwapInterval(-1);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	
	cursorProgram = loadCursorShaders();
	glGenVertexArrays(1, &cursorVAO);
	GlId cursorVBO;
	glGenBuffers(1, &cursorVBO);
	
	glBindVertexArray(cursorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cursorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cursorVertices), cursorVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	
	world.players.emplace_back(world, glm::vec3(8.0f, 50.0f, 8.0f));
	player = &world.players.back();
	
	input.init(window);
	input.capturingMouse(!paused);
	BlockRegistry::registerBlocks();
	blockRenderer.init();
	textRenderer.init();
	
	debugText = &textRenderer.createText("Loading debug...", 5, 5, 1.0, glm::vec4(1, 1, 1, 1));
	
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
	double lastFrame, lastSecond, now;
	while(!glfwWindowShouldClose(window)) {
		update(dt);
		render();
		
		now = glfwGetTime();
		dt = now - lastFrame;
		lastFrame = now;
		
		if(firstFrame) firstFrame = false;
		
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

void GameClient::update(float dt) {
	glfwPollEvents();
	
	if(!paused) {
		if(input.justPressed(GLFW_KEY_F)) {
			if(player->movementMode == MovementMode::flying) {
				player->movementMode = MovementMode::normal;
			} else {
				player->movementMode = MovementMode::flying;
			}
		}
		if(input.justPressed(GLFW_KEY_N)) {
			if(player->movementMode == MovementMode::noClip) {
				player->movementMode = MovementMode::normal;
			} else {
				player->movementMode = MovementMode::noClip;
			}
		}
		
		player->move(input.getMovementKeys(), dt);
		
		glm::vec2 mouseMvt = input.getMouseMovement();
		player->rotate(glm::vec3(-mouseMvt.y, -mouseMvt.x, 0));
		
		bool click1 = input.justClicked(1);
		bool click2 = input.justClicked(2);
		if(click1 || click2) {
			bool hit;
			int x, y, z;
			std::tie(hit, x, y, z) = player->castRay(5, !click1);
			if(hit) {
				int32_t chunkX, chunkZ;
				std::tie(chunkX, chunkZ) = world.getChunkAt(x, z);
				if(click2) {
					world.setBlock(x, y, z, Block::fromId(1));
				} else {
					world.removeBlock(x, y, z);
				}
				Chunk& chunk = world.getChunk(chunkX, chunkZ);
				blockRenderer.renderChunk(chunk, chunkX, chunkZ);
			}
		}
	}
	input.clearJustClicked();
	
	if(input.justPressed(GLFW_KEY_ESCAPE)) {
		paused = !paused;
		input.capturingMouse(!paused);
	}
	input.clearJustPressed();
	
	int32_t camChunkX = floor(player->pos().x / CHUNK_SIZE);
	int32_t camChunkZ = floor(player->pos().z / CHUNK_SIZE);
	int loads = 0;
	SpiralIterator iter(camChunkX, camChunkZ);
	while(iter.withinDistance(RENDER_DIST)) {
		int x = iter.getX();
		int z = iter.getZ();
		if(!world.isChunkLoaded(x, z)) {
			Chunk& chunk = world.genChunk(x, z);
			blockRenderer.renderChunk(chunk, x, z);
			loads++;
			if(loads >= LOADS_PER_FRAME) break;
		}
		iter.next();
	}
}

void GameClient::render() {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	float aspect = ((float) width) / height;
	glm::mat4 proj = glm::perspective(glm::radians(90.0f), aspect, 0.001f, 1000.0f);
	glm::mat4 view = globalToLocal(player->pos(), player->orient());
	
	glClearColor(SKY_COLOR[0], SKY_COLOR[1], SKY_COLOR[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	
	glm::vec3 playerPos = player->pos();
	int32_t camChunkX = floor(playerPos.x / CHUNK_SIZE);
	int32_t camChunkZ = floor(playerPos.z / CHUNK_SIZE);
	blockRenderer.render(proj, view, camChunkX, camChunkZ, SKY_COLOR);
	
	glLineWidth(2.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glUseProgram(cursorProgram);
	glUniform2f(glGetUniformLocation(cursorProgram, "winSize"), width, height);
	glBindVertexArray(cursorVAO);
	glDrawArrays(GL_LINES, 0, 4);
	glBindVertexArray(0);
	glUseProgram(0);
	
	debugText->setText(std::to_string(FPS) + " FPS\n"
		+ ("Pos: " + vec3ToString(playerPos) + "\n")
		+ (std::string("Mode: ") + movementModeNames[static_cast<int>(player->movementMode)]));
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	textRenderer.render();
}
