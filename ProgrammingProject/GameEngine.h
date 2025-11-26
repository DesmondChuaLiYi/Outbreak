#ifndef GAMEENGINE_H
#define GAMEENGINE_H
#include <string>
#include <vector>
#include <windows.h>
#include "Location.h"
#include "Player.h"
#include "ClueJournal.h"

class GameEngine {
private:
	// Singleton instance
	static GameEngine* instance;

	// Private constructor (singleton pattern)
	GameEngine();

	// Game state
	Player* currentPlayer;
	Location* currentLocation;
	ClueJournal* journal;
	std::vector<Location*> allLocations;

	// Story progression
	int currentChapter;
	bool gameRunning;

	// Temporary storage for loaded game state
	int savedExplorationProgress;
	int savedMovementSteps;

	// Helper methods for initialization
	void initializeAllLocations();
	void initializeAllLoreItems();
	Location* createLocation(const std::string& id, const std::string& name,
		Location::LocationType type, const std::string& description,
		const std::string& atmosphere, Location::Hazard hazard, int hazardDamage,
		int chapterNumber, const std::string& chapterTitle, const std::string& chapterStory);

public:
	// Console dimensions - SINGLE SOURCE OF TRUTH
	static const int CONSOLE_WIDTH = 120;
	static const int CONSOLE_HEIGHT = 150;

	// Delete copy constructor and assignment operator
	GameEngine(const GameEngine&) = delete;
	GameEngine& operator=(const GameEngine&) = delete;

	// Get singleton instance
	static GameEngine* getInstance();

	// Initialize the game engine
	bool initialize();

	// Setup console window
	void setupConsoleWindow();

	// Game state management
	void setPlayer(Player* player);
	Player* getPlayer();
	void setCurrentLocation(Location* location);
	Location* getCurrentLocation();
	ClueJournal* getJournal();

	// Location management
	Location* getLocationByID(const std::string& locationID);
	std::vector<Location*>& getAllLocations();

	// Story progression
	void displayChapterIntro();
	int getCurrentChapter() const;
	void advanceChapter();

	// Game loop control
	bool isGameRunning() const;
	void setGameRunning(bool running);

	// Traversal
	bool travelToLocation(const std::string& locationID);
	void displayTravelOptions();

	// Game flow methods
	Player* createCharacter();
	void runGame();
	void runExplorationLoop(Player* player); // Main exploration gameplay loop
	int displayGameMenu();
	void gameLoop(Player* player);
	void handleMenuChoice(int choice, Player* player, bool& running);

	// Save/Load system
	bool saveGame(Player* player, int slotNumber);
	Player* loadGame(int slotNumber);
	int selectSaveSlot(bool isLoading);
	void displaySaveSlots();
	bool deleteSaveSlot(int slotNumber);

	// Main menu handlers
	void handleNewGame();
	void handleLoadGame();

	// Skill tree UI
	void displaySkillTreeMenu(Player* player);

	// Cleanup and destroy singleton
	static void destroyInstance();

	// Destructor
	~GameEngine();
};

#endif /* GAMEENGINE_H */
