#include "NavigationMenu.h"
#include <iostream>
#include <cstdlib>

#ifdef _WIN32
	#define CLEAR_SCREEN "cls"
#else
	#define CLEAR_SCREEN "clear"
#endif

NavigationMenu::NavigationMenu() : selectedIndex(0), lastRenderedIndex(-1), isRunning(false) {
	// Initialize
}

NavigationMenu::~NavigationMenu() {
	// Cleanup
}

void NavigationMenu::clearScreen() {
	system(CLEAR_SCREEN);
}

void NavigationMenu::centerText(const std::string& text) {
	int padding = (120 - text.length()) / 2;
	std::cout << std::string(padding, ' ') << text;
}

void NavigationMenu::drawSeparator() {
	std::cout << std::string(120, '=') << "\n";
}

bool NavigationMenu::handleInput(int maxOptions) {
	#ifdef _WIN32
		bool selectionChanged = false;

		if (GetAsyncKeyState(VK_UP) & 0x8000) {
			if (selectedIndex > 0) {
				selectedIndex--;
				selectionChanged = true;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}

		if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
			if (selectedIndex < maxOptions - 1) {
				selectedIndex++;
				selectionChanged = true;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}

		if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
			isRunning = false;
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}

		return selectionChanged;
	#else
		return false;
	#endif
}

void NavigationMenu::renderMenuOptions(const std::string& title, const std::vector<std::string>& options) {
	std::cout << "\n";
	drawSeparator();
	std::cout << "\n";

	// Title
	centerText(title + "\n");
	std::cout << "\n";
	drawSeparator();
	std::cout << "\n\n";

	// Options
	for (size_t i = 0; i < options.size(); ++i) {
		std::string option = "[" + std::to_string(i + 1) + "] " + options[i];

		if (i == selectedIndex) {
			std::cout << "     ";
			centerText("[>>] " + option + " [<<]\n");
			std::cout << "\n";
		}
		else {
			std::cout << "     ";
			centerText("[ ] " + option + "\n");
			std::cout << "\n";
		}
	}

	std::cout << "\n";
	drawSeparator();
	std::cout << "\n";
	centerText("Use UP/DOWN arrows | ENTER to select\n");
	std::cout << "\n";
	drawSeparator();

	lastRenderedIndex = selectedIndex;
}

int NavigationMenu::displayMenu(const std::string& title, const std::vector<std::string>& options) {
	if (options.empty()) {
		return -1;
	}

	selectedIndex = 0;
	lastRenderedIndex = -1;
	isRunning = true;

	// Initial full render
	clearScreen();
	std::cout << "\n";
	renderMenuOptions(title, options);

	while (isRunning) {
		bool selectionChanged = handleInput(options.size());

		// Re-render menu if selection changed
		if (selectionChanged) {
			clearScreen();
			std::cout << "\n";
			renderMenuOptions(title, options);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	return selectedIndex;
}

void NavigationMenu::pauseScreen(const std::string& message) {
	std::cout << "\n";
	centerText(message + "\n");
	std::cin.ignore();
}

std::string NavigationMenu::getCurrentTitle() const {
	return "";
}

std::vector<std::string> NavigationMenu::getCurrentOptions() const {
	return std::vector<std::string>();
}
