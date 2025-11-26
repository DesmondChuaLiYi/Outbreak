#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include "GameEngine.h"

class TitleScreen {
private:
	// Core members
	std::string gameTitle;
	std::vector<std::string> menuOptions;
	int selectedIndex;
	int lastRenderedIndex;  // Track last rendered index for surgical updates
	bool isRunning;

public:
	// Constructor
	TitleScreen(const std::string& title = "ZOMBIE APOCALYPSE SURVIVAL");

	// Destructor
	~TitleScreen();


	// Helper methods
	void centerText(const std::string& text);
	void leftAlignText(const std::string& text, int leftPadding);
	void drawSeparator();
	void drawThemedSeparator();
	void drawTitle();
	void drawAtmosphere();
	void drawMenuOptions();
	void drawFooter();

	// Public interface methods
	void clearScreen();
	void renderMenu();
	void updateMenuSelection();
	void handleInput();
	bool initialize(const std::vector<std::string>& options);
	int run();
};

#endif /* TITLESCREEN_H */
