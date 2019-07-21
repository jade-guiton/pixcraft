#include "client.hpp"

const char windowTitle[] = "OpenGL Test 2";

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	glfwMakeContextCurrent(window);
	glViewport(0, 0, width, height);
	
	GameClient* client = (GameClient*) glfwGetWindowUserPointer(window);
	client->width = width; client->height = height;
	
	std::cout << "Window resized to " << width << "×" << height << std::endl;
}

GameClient::GameClient() : firstFrame(true) {
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
	
	BlockTypes::registerTypes();
	
	blockRenderer.init();
	
	for(int32_t x = -1; x <= 1; ++x) {
		for(int32_t z = -1; z <= 1; ++z) {
			Chunk& chunk = world.genChunk(x, z);
			blockRenderer.renderChunk(chunk, x, z);
		}
	}
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwGetCursorPos(window, &oldMouseX, &oldMouseY);
}

GameClient::~GameClient() {
	glfwDestroyWindow(window);
}

constexpr float GameClient::SKY_COLOR[];

void GameClient::mainLoop() {
	while(!glfwWindowShouldClose(window)) {
		update();
		render();
		firstFrame = false;
	}
}

glm::mat4 globalToLocal(glm::vec3 pos, glm::vec3 orient) {
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::rotate(mat, -orient.z, glm::vec3(0.0f, 0.0f, 1.0f));
	mat = glm::rotate(mat, -orient.x, glm::vec3(1.0f, 0.0f, 0.0f));
	mat = glm::rotate(mat, -orient.y, glm::vec3(0.0f, 1.0f, 0.0f));
	mat = glm::translate(mat, -pos);
	return mat;
}

void GameClient::update() {
	glfwPollEvents();
	
	glm::mat4 planeRot = glm::mat4(1.0f);
	planeRot = glm::rotate(planeRot, world.playerOrient.y, glm::vec3(0.0f, 1.0f, 0.0f));
	planeRot = glm::mat3(planeRot);
	float speed = 0.2;
	int dx = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) - (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
	int dy = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) - (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
	int dz = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) - (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
	if(dx != 0 || dy != 0 || dz != 0) {
		glm::vec4 mov = glm::vec4(glm::normalize(glm::vec3(dx, dy, dz)), 1.0f);
		world.playerPos += speed * glm::vec3(planeRot * mov);
	}
	
	double sensitivity = 0.005;
	double newMouseX, newMouseY;
	glfwGetCursorPos(window, &newMouseX, &newMouseY);
	if(!firstFrame) {
		world.playerOrient.y -= (newMouseX-oldMouseX) * sensitivity;
		world.playerOrient.x -= (newMouseY-oldMouseY) * sensitivity;
	}
	oldMouseX = newMouseX;
	oldMouseY = newMouseY;
}

void GameClient::render() {
	//std::cout << "Position: " << world.playerPos.x << " " << world.playerPos.z << std::endl;
	
	glm::mat4 proj = glm::perspective(glm::radians(90.0f), ((float) width) / height, 0.001f, 1000.0f);
	glm::mat4 view = globalToLocal(world.playerPos, world.playerOrient);
	
	glClearColor(SKY_COLOR[0], SKY_COLOR[1], SKY_COLOR[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	int32_t camChunkX = world.playerPos.x / CHUNK_SIZE;
	int32_t camChunkZ = world.playerPos.z / CHUNK_SIZE;;
	blockRenderer.render(proj, view, camChunkX, camChunkZ, SKY_COLOR);
	
	glfwSwapBuffers(window);
}
