#ifndef AISTORYTELLER_H
#define AISTORYTELLER_H

#include <string>
#include <vector>

class Player;
class Location;

class AIStoryteller {
private:
	static AIStoryteller* instance;
	
	// Criteria 1: Time tracking
	int gameTimeSeconds;
	int totalMoves;
	
	// Criteria 2: Player health tracking
	float playerHealthRatio;
	int playerLevel;
	
	// Difficulty state
	float difficultyMultiplier;
	float tensionLevel;  // 0.0 to 1.0
	
	// Event tracking
	int movesSinceLastEvent;
	int eventCooldown;
	
	// Clue tracking (ensure all clues spawn)
	std::vector<int> spawnedClueIDs;
	std::vector<int> availableClueIDs;
	int clueSpawnCooldown;
	
	AIStoryteller();
	
	void calculateTension();
	void updateDifficulty();
	
public:
	static AIStoryteller* getInstance();
	static void destroyInstance();
	
	// Update storyteller state based on criteria
	void update(Player* player, int moves);
	void incrementTime(int seconds);
	void initializeClues(const std::vector<int>& allClueIDs);
	
	// NEW: Spawn decision methods (replace hardcoded percentages)
	bool shouldSpawnZombie();
	bool shouldSpawnLoot();
	bool shouldSpawnClue();
	
	// NEW: Zombie difficulty scaling
	int getZombieWaveCount();  // More waves when high HP
	std::string getZombieType();  // Tougher zombies when high HP
	
	// NEW: Random loot generation
	std::string generateRandomLoot();  // Returns item type
	int getLootQuantity();
	
	// NEW: Random clue generation
	int generateRandomClue();  // Returns clue ID, -1 if none
	
	// ASPECT 1: Zombie spawn influence
	int adjustZombieCount(int baseCount);
	float getZombieDifficultyModifier();
	bool shouldSpawnSpecialZombie();
	
	// ASPECT 2: Loot drop influence
	float getLootQualityModifier();
	int getBonusLootChance();
	bool shouldGrantBonusLoot();
	
	// ASPECT 3: Random event influence
	bool shouldTriggerEvent();
	std::string generateRandomEvent();
	void handleEvent(const std::string& eventType);
	
	// Debug/info
	std::string getStorytellerStatus();
	float getTensionLevel() const { return tensionLevel; }
	
	~AIStoryteller();
};

#endif /* AISTORYTELLER_H */
