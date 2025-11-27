#ifndef GAMEPLAYENGINE_H
#define GAMEPLAYENGINE_H

#include "Player.h"
#include "Location.h"
#include "Zombie.h"
#include "Item.h"
#include "ClueJournal.h"
#include "Queue.h"
#include "Stack.h"
#include "Crafting.h"
#include <string>
#include <vector>

// ============================================================================
// DIRECTION ENUM
// ============================================================================
enum class Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	CENTER
};

// ============================================================================
// LOOT STRUCT - Item at specific location
// ============================================================================
struct Loot {
	std::string lootID;
	Item item;
	Direction location;
	bool isPickedUp;

	Loot(const std::string& id, const Item& it, Direction dir)
		: lootID(id), item(it), location(dir), isPickedUp(false) {
	}
};

// ============================================================================
// CLUE LOCATION STRUCT - Lore item at specific location
// ============================================================================
struct ClueLocation {
	int clueID;
	std::string clueName;
	Direction location;
	bool collected;

	ClueLocation(int id, const std::string& name, Direction dir)
		: clueID(id), clueName(name), location(dir), collected(false) {
	}
};

// ============================================================================
// COMBAT RESULT STRUCT - Results from a combat encounter
// ============================================================================
struct CombatResult {
	bool playerWon;
	int playerDamageDealt;
	int playerDamageTaken;
	int zombiesKilled;
	int zombiesRemaining;

	CombatResult()
		: playerWon(false), playerDamageDealt(0), playerDamageTaken(0),
		zombiesKilled(0), zombiesRemaining(0) {
	}
};

// ============================================================================
// GAMEPLAY ENGINE - Core exploration, combat, and progression
// ============================================================================
class GameplayEngine {
private:
	// Singleton instance
	static GameplayEngine* instance;

	// Game state
	Player* currentPlayer;
	Location* currentLocation;
	ClueJournal* journal;

	// Combat state
	Queue<Zombie*> currentWave;
	int currentWaveNumber;
	int maxWavesPerLocation;

	// Combat history
	Stack<std::string> combatActionHistory;
	static const int MAX_COMBAT_HISTORY = 5;

	// Movement & exploration state
	int movementSteps;
	int stepsToNewLocation;  // Now 12 instead of 15

	// Location state
	std::vector<Loot> currentLocationLoot;
	std::vector<ClueLocation> currentLocationClues;
	std::vector<std::string> pickedUpLootIDs; // Track picked up loot globally
	bool inCombat;
	bool hasExploredNewArea;

	// Private constructor
	GameplayEngine();

	// Delete copy constructor and assignment operator
	GameplayEngine(const GameplayEngine&) = delete;
	GameplayEngine& operator=(const GameplayEngine&) = delete;

	// Private helper methods
	void spawnZombieWave();
	void populateLocationLoot();
	void populateLocationClues();
	void displayLocationLayout();
	void checkForLoot(Direction direction);
	void checkForClue(Direction direction);
	void checkForLocationTransition();
	void playerAttack(Zombie* zombie, bool isRanged = false);
	int playerDodge();
	bool playerUseItem(const std::string& itemName);
	Zombie* getNextZombie();

public:
	// Destructor
	~GameplayEngine();

	// Singleton pattern
	static GameplayEngine* getInstance();
	static void destroyInstance();

	// ========================================================================
	// INITIALIZATION
	// ========================================================================
	void initialize(Player* player, Location* location, ClueJournal* journal);
	void setCurrentLocation(Location* location);

	// ========================================================================
	// EXPLORATION SYSTEM
	// ========================================================================
	void displayCurrentLocation();
	void moveInDirection(Direction direction);
	void checkForHazard();
	void displayLootOptions();
	void interactWithLoot(Direction direction);
	void interactWithClue(int clueIndex);
	bool canTravelToNewLocation() const;
	void displayTravelOptions();
	bool travelToLocation(const std::string& locationID);

	// AI Storyteller integration
	void spawnAILoot(const std::string& lootType, int quantity, Direction direction);
	void spawnAIClue(int clueID);

	// ========================================================================
	// COMBAT SYSTEM
	// ========================================================================
	void startCombat();
	CombatResult conductCombat();
	void handleCombatRound();
	void processZombieDeath(Zombie* zombie);
	void endCombat(const CombatResult& result);

	// ========================================================================
	// UI & DISPLAY
	// ========================================================================
	Direction stringToDirection(const std::string& input);
	std::string directionToString(Direction direction);
	void displayPlayerStatus();
	void displayInventory();
	void displayCollectedClues();

	// ========================================================================
	// GETTERS
	// ========================================================================
	Player* getPlayer() const { return currentPlayer; }
	Location* getCurrentLocation() const { return currentLocation; }
	bool isInCombat() const { return inCombat; }
	int getMovementSteps() const { return movementSteps; }
	int getExplorationProgress() const { return stepsToNewLocation; }

	// Setters for save/load
	void setMovementSteps(int steps) { movementSteps = steps; }
	void setExplorationProgress(int progress) { stepsToNewLocation = progress; }
	void addPickedUpLootID(const std::string& lootID) { pickedUpLootIDs.push_back(lootID); }
	void setPickedUpLootIDs(const std::vector<std::string>& ids) { pickedUpLootIDs = ids; }

	// Getters for save/load
	size_t getPickedUpLootCount() const { return pickedUpLootIDs.size(); }
	const std::vector<std::string>& getPickedUpLootIDs() const { return pickedUpLootIDs; }
	bool isLootPickedUp(const std::string& lootID) const {
		for (const auto& id : pickedUpLootIDs) {
			if (id == lootID) return true;
		}
		return false;
	}
};

#endif /* GAMEPLAYENGINE_H */