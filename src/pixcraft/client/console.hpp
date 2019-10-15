#pragma once

#include <string>
#include <deque>
#include <functional>
#include <unordered_map>

#include "input.hpp"
#include "text.hpp"

namespace PixCraft {
	class Console {
	public:
		Console();
		
		bool isOpen();
		
		void resetCommands();
		void addCommand(std::string command, std::function<void()> callback);
		
		void write(std::string line);
		void clearHistory();
		
		void update(InputManager& input);
		void render(TextRenderer& textRenderer, int winW, int winH);
		
	private:
		bool open;
		std::deque<std::string> history;
		std::unordered_map<std::string, std::function<void()>> commands;
		
		std::string inputBuffer;
		uint32_t cursorPos;
		
		int framesSinceUpdate;
		
		void executeCommand(std::string command);
	};
}
