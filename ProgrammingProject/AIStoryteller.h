#ifndef AISTORYTELLER_H
#define AISTORYTELLER_H

#include <string>

class Player;

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
	
	AIStoryteller();
	
	void calculateTension();
	void updateDifficulty();
	
public:
	static AIStoryteller* getInstance();
	static void destroyInstance();
	
	// Update storyteller state based on criteria
	void update(Player* player, int moves);
	void incrementTime(int seconds);
	
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
