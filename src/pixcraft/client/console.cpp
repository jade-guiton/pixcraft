#include "console.hpp"

#include <iostream>

#include "utf8.h"

using namespace PixCraft;

Console::Console() : open(false), cursorPos(0) {}

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
		if(inputText.size() > 0) {
			inputBuffer.insert(cursorPos, inputText);
			cursorPos += inputText.size();
		}
		
		bool left = input.justPressed(GLFW_KEY_LEFT);
		bool backspace = input.justPressed(GLFW_KEY_BACKSPACE);
		if((left || backspace) && cursorPos > 0) {
			auto it = inputBuffer.begin() + cursorPos;
			utf8::prior(it, inputBuffer.begin());
			if(backspace)
				inputBuffer.erase(it, inputBuffer.begin() + cursorPos);
			cursorPos = it - inputBuffer.begin();
		}
		
		bool right = input.justPressed(GLFW_KEY_RIGHT);
		bool delete_ = input.justPressed(GLFW_KEY_DELETE);
		if((right || delete_) && cursorPos < inputBuffer.size()) {
			auto it = inputBuffer.begin() + cursorPos;
			utf8::next(it, inputBuffer.end());
			if(delete_)
				inputBuffer.erase(inputBuffer.begin() + cursorPos, it);
			else
				cursorPos = it - inputBuffer.begin();
		}
		
		if(input.justPressed(GLFW_KEY_ENTER)) {
			std::string command = inputBuffer;
			write("> " + command);
			executeCommand(command);
		}
		
		if(input.justPressed(GLFW_KEY_ESCAPE) || input.justPressed(GLFW_KEY_ENTER)) {
			open = false;
			inputBuffer.clear();
			cursorPos = 0;
			input.capturingMouse(true);
		}
	} else {
		if(input.justPressed(GLFW_KEY_ENTER)) {
			open = true;
			input.capturingMouse(false);
		}
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
		std::string prompt = ">> ";
		std::string inputLine = prompt + inputBuffer;
		textRenderer.renderText(inputLine, x, y, glm::vec3(1.0, 1.0, 1.0));
		std::string beforeCursor = inputLine.substr(0, prompt.size() + cursorPos);
		std::string cursor = "|";
		int cursorX = x + textRenderer.getTextWidth(beforeCursor) - textRenderer.getTextWidth(cursor)/2;
		textRenderer.renderText(cursor, cursorX, y, glm::vec3(1.0, 1.0, 1.0));
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
