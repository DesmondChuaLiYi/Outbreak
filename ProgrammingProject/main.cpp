#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "GameEngine.h"
#include "AudioEngine.h"
#include "GameplayEngine.h"
#include "TitleScreen.h"

int main() {
	// Initialize singletons
	GameEngine* engine = GameEngine::getInstance();
	AudioEngine* audio = AudioEngine::getInstance(); // Auto-plays background music

	// Setup game
	engine->initialize();

	// Main game loop - returns to title screen after each session
	bool gameRunning = true;
	while (gameRunning) {
		// Display title screen
		TitleScreen titleScreen("OUTBREAK");
		int choice = titleScreen.run();

		switch (choice) {
		case 0: // New Game
			engine->handleNewGame();
			break;

		case 1: // Load Game
			engine->handleLoadGame();
			break;

		case 2: // Exit
			system("cls");
			std::cout << "\n\n  Thanks for playing OUTBREAK.\n";
			std::cout << "  Stay safe, survivor...\n\n";
			std::this_thread::sleep_for(std::chrono::milliseconds(1500));
			gameRunning = false;
			break;
		}
	}

	// Cleanup singletons
	audio->stopBackgroundMusic();
	AudioEngine::destroyInstance();
	GameplayEngine::destroyInstance();
	GameEngine::destroyInstance();

	return 0;
}