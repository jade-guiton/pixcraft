#include "client.hpp"

const char windowTitle[] = "OpenGL Test 2";

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	std::cout << "Window resized to " << width << "×" << height << std::endl;
}

GameClient::GameClient()
	: blockRenderer(RENDER_DIST), paused(false), firstFrame(true), FPS(0.0) {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	window = glfwCreateWindow(START_WIDTH, START_HEIGHT, windowTitle, nullptr, nullptr);
	if(window == nullptr)
		throw std::runtime_error("Failed to create GLFW window");
	
	glfwMakeContextCurrent(window);
	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
		throw std::runtime_error("Failed to initialize GLAD");
	
	glfwSetFramebufferSizeCallback(window, windowResizeCallback);
	windowResizeCallback(window, START_WIDTH, START_HEIGHT);
	glfwSwapInterval(-1);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	input.init(window);
	input.capturingMouse(!paused);
	BlockRegistry::registerBlocks();
	blockRenderer.init();
	
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
			std::cout << "FPS: " << FPS << std::endl;
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

void GameClient::update(float dt) {
	glfwPollEvents();
	
	if(!paused) {
		glm::mat4 yRot = glm::mat4(1.0f);
		yRot = glm::rotate(yRot, world.playerOrient.y, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec3 mvt = input.getMovement();
		world.playerPos += dt * MVT_SPEED * glm::vec3(yRot * glm::vec4(mvt, 1.0f));
		
		glm::vec2 mouseMvt = input.getMouseMovement();
		if(!firstFrame) {
			world.playerOrient.y -= mouseMvt.x;
			world.playerOrient.x -= mouseMvt.y;
		}
	}
	
	if(input.justPressed(GLFW_KEY_ESCAPE)) {
		paused = !paused;
		input.capturingMouse(!paused);
	}
	input.clearJustPressed();
	
	int32_t camChunkX = floor(world.playerPos.x / CHUNK_SIZE);
	int32_t camChunkZ = floor(world.playerPos.z / CHUNK_SIZE);
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
	glm::mat4 view = globalToLocal(world.playerPos, world.playerOrient);
	
	glClearColor(SKY_COLOR[0], SKY_COLOR[1], SKY_COLOR[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	int32_t camChunkX = floor(world.playerPos.x / CHUNK_SIZE);
	int32_t camChunkZ = floor(world.playerPos.z / CHUNK_SIZE);
	blockRenderer.render(proj, view, camChunkX, camChunkZ, SKY_COLOR);
}
