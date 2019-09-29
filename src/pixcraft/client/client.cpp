#include "client.hpp"

#include <iostream>
#include <stdexcept>

#include "play_state.hpp"
#include "menu_state.hpp"

#include "pixcraft/util/version.hpp"

using namespace PixCraft;

void glfwErrorCallback(int error, const char* desc) {
	std::cout << "GLFW error " << error << ": " << desc << std::endl;
}

GameState::GameState(GameClient& client) : client(client) {}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	std::cout << "Window resized to " << width << "×" << height << std::endl;
	
	GameClient& client = *((GameClient*) glfwGetWindowUserPointer(window));
	client.getTextRenderer().setViewport(width, height);
}

GameClient::GameClient() : frameNo(0), FPS(0.0) {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	
	std::string title = "PixCraft " + getVersionString();
	window = glfwCreateWindow(START_WIDTH, START_HEIGHT, title.c_str(), nullptr, nullptr);
	if(window == nullptr)
		throw std::runtime_error("Failed to create GLFW window");
	glfwSetWindowUserPointer(window, (void*) this);
	
	glfwMakeContextCurrent(window);
	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
		throw std::runtime_error("Failed to initialize GLAD");
	
	std::cout << "Using OpenGL version " << GLVersion.major << "." << GLVersion.minor << std::endl;
	
	glfwSetFramebufferSizeCallback(window, windowResizeCallback);
	windowResizeCallback(window, START_WIDTH, START_HEIGHT);
	glfwSwapInterval(-1);
	
	textRenderer.init();
	input.init(window);
	
	TextureManager::loadTextures();
	BlockRegistry::defineBlocks();
	Button::initRendering();
	
	gameState.reset(new MenuState(*this));
	
	glfwSetTime(0.0);
}

GameClient::~GameClient() {
	textRenderer.free();
	glfwDestroyWindow(window);
}

void GameClient::run() {
	glfwSetTime(0.0);
	
	int frameCounter = 0;
	
	float dt = 1 / 60.0;
	double now = glfwGetTime();
	double lastFrame = now;
	double lastSecond = now;
	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		gameState->update(dt);
		input.clearJustClicked();
		input.clearJustScrolled();
		input.clearJustPressed();
		
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		gameState->render(width, height);
		
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

int GameClient::getFPS() {
	return FPS;
}

int main() {
	glfwSetErrorCallback(glfwErrorCallback);
	glfwInit();
	
	try {
		GameClient client;
		client.run();
	} catch(std::runtime_error& err) {
		std::cout << "A runtime error occured: " << err.what() << std::endl;
		glfwTerminate();
		return 1;
	}
	
	glfwTerminate();
	return 0;
}
