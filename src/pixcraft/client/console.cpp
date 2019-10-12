#include "console.hpp"

#include <iostream>

using namespace PixCraft;

Console::Console() : open(false) {}

bool Console::isOpen() { return open; }

void Console::resetCommands() {
	commands.clear();
}

void Console::addCommand(std::string command, std::function<void()> callback) {
	commands[command] = callback;
}

void Console::write(std::string line) {
	history.push_back(line);
}

void Console::clearHistory() {
	history.clear();
}

void Console::update(InputManager& input) {
	if(open) {
		std::string inputText = input.inputBuffer();
		if(inputText.size() > 0) inputBuffer.append(inputText);
		
		if(input.justPressed(GLFW_KEY_ENTER)) {
			std::string command = inputBuffer;
			write("> " + command);
			executeCommand(command);
		}
		
		if(input.justPressed(GLFW_KEY_ESCAPE) || input.justPressed(GLFW_KEY_ENTER)) {
			open = false;
			inputBuffer.clear();
		}
	} else {
		if(input.justPressed(GLFW_KEY_ENTER)) open = true;
	}
}

void Console::render(TextRenderer& textRenderer, int winW, int winH) {
	int lineHeight = textRenderer.getTextHeight()*2;
	int x = -winW/2 + 10;
	int y = -winH/2 + 10 + history.size()*lineHeight;
	for(auto& line : history) {
		textRenderer.renderText(line, x, y, glm::vec3(1.0, 1.0, 1.0));
		y -= lineHeight;
	}
	if(open) {
		textRenderer.renderText(">> " + inputBuffer, x, y, glm::vec3(1.0, 1.0, 1.0));
	}
	checkGlErrors("command line rendering");
}

void Console::executeCommand(std::string command) {
	auto it = commands.find(command);
	if(it != commands.end()) {
		it->second();
	} else {
		write("Unrecognized command: '" + command + "'");
	}
}
