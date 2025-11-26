#ifndef NAVIGATIONMENU_H
#define NAVIGATIONMENU_H
#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <Windows.h>

class NavigationMenu {
public:
	// Constructor
	NavigationMenu();

	// Destructor
	~NavigationMenu();

	// Display a menu and return selected index
	int displayMenu(const std::string& title, const std::vector<std::string>& options);

	// Clear screen utility
	void clearScreen();

	// Pause for user input
	void pauseScreen(const std::string& message = "Press ENTER to continue...");

	// Center text on screen
	void centerText(const std::string& text);

	// Draw separator
	void drawSeparator();

private:
	int selectedIndex;
	int lastRenderedIndex;
	bool isRunning;

	// Input handling
	bool handleInput(int maxOptions);
	void renderMenuOptions(const std::string& title, const std::vector<std::string>& options);
	std::string getCurrentTitle() const;
	std::vector<std::string> getCurrentOptions() const;
};

#endif /* NAVIGATIONMENU_H */
