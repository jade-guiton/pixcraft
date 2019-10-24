#include "client.hpp"

#include <iostream>
#include <stdexcept>
#include <algorithm>

#include "play_state.hpp"
#include "menu_state.hpp"

#include "pixcraft/util/version.hpp"

using namespace PixCraft;

void glfwErrorCallback(int error, const char* desc) {
	std::cout << "GLFW error " << error << ": " << desc << std::endl;
}

GameState::GameState(GameClient& client) : client(client) {}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	GameClient& client = *((GameClient*) glfwGetWindowUserPointer(window));
	client.setViewportSize(width, height);
}

GameClient::GameClient() : width(START_WIDTH), height(START_HEIGHT), nextGameState(nullptr), frameNo(0), FPS(0.0), fullscreen(false) {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	
	std::string title = "PixCraft " + getVersionString();
	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if(window == nullptr)
		throw std::runtime_error("Failed to create GLFW window");
	glfwSetWindowUserPointer(window, (void*) this);
	
	glfwMakeContextCurrent(window);
	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
		throw std::runtime_error("Failed to initialize GLAD");
	
	std::cout << "Using OpenGL version " << GLVersion.major << "." << GLVersion.minor << std::endl;
	
	input.init(window);
	textRenderer.init();
	
	glfwSetFramebufferSizeCallback(window, windowResizeCallback);
	windowResizeCallback(window, width, height);
	glfwSwapInterval(-1);
	
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

GLFWmonitor* getCurrentMonitor(GLFWwindow *window) {
	int wx, wy, ww, wh;
	glfwGetWindowPos(window, &wx, &wy);
	glfwGetWindowSize(window, &ww, &wh);
	
	int nmonitors;
	GLFWmonitor** monitors = glfwGetMonitors(&nmonitors);
	
	GLFWmonitor* bestmonitor = nullptr;
	int bestoverlap = 0;
	
	for(int i = 0; i < nmonitors; ++i) {
		const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
		int mx, my, mw, mh;
		glfwGetMonitorPos(monitors[i], &mx, &my);
		mw = mode->width;
		mh = mode->height;
		
		int overlap = std::max(0, std::min(wx + ww, mx + mw) - std::max(wx, mx))
			* std::max(0, std::min(wy + wh, my + mh) - std::max(wy, my));
		
		if(bestoverlap < overlap) {
			bestoverlap = overlap;
			bestmonitor = monitors[i];
		}
	}

	return bestmonitor;
}

void GameClient::run() {
	glfwSetTime(0.0);
	
	int frameCounter = 0;
	
	float dt = 1 / 60.0;
	double now = glfwGetTime();
	double lastFrame = now;
	double lastSecond = now;
	while(!glfwWindowShouldClose(window)) {
		if(nextGameState) {
			gameState.reset(nextGameState);
			nextGameState = nullptr;
		}
		
		glfwPollEvents();
		gameState->update(dt);
		if(input.justPressed(GLFW_KEY_F11)) {
			fullscreen = !fullscreen;
			if(fullscreen) {
				windowedWidth = width;
				windowedHeight = height;
				GLFWmonitor* monitor = getCurrentMonitor(window);
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
			} else {
				GLFWmonitor* monitor = glfwGetWindowMonitor(window);
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				glfwSetWindowMonitor(window, nullptr, (mode->width - windowedWidth)/2, (mode->height - windowedHeight)/2,
					windowedWidth, windowedHeight, GLFW_DONT_CARE);
			}
		}
		input.clearAll();
		
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

void GameClient::stop() {
	glfwSetWindowShouldClose(window, true);
}

InputManager& GameClient::getInputManager() { return input; }
TextRenderer& GameClient::getTextRenderer() { return textRenderer; }

void GameClient::setViewportSize(int width2, int height2) {
	if(width2 > 0 && height2 > 0) { // Ignore dimensions = (0, 0) on minimize
		width = width2;
		height = height2;
		glViewport(0, 0, width, height);
		textRenderer.setViewport(width, height);
		std::cout << "Window resized to " << width << "×" << height << std::endl;
	}
}

void GameClient::setGameState(GameState* newGameState) {
	nextGameState = newGameState;
}

int GameClient::getFrameNo() { return frameNo; }
int GameClient::getFPS() { return FPS; }

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
