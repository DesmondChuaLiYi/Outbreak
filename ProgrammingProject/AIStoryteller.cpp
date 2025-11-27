#include "AIStoryteller.h"
#include "Player.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

AIStoryteller* AIStoryteller::instance = nullptr;

AIStoryteller::AIStoryteller() 
	: gameTimeSeconds(0), totalMoves(0), playerHealthRatio(1.0f), 
	  playerLevel(1), difficultyMultiplier(1.0f), tensionLevel(0.0f),
	  movesSinceLastEvent(0), eventCooldown(0), clueSpawnCooldown(0) {
	srand(static_cast<unsigned int>(time(0)));
}

AIStoryteller* AIStoryteller::getInstance() {
	if (!instance) {
		instance = new AIStoryteller();
	}
	return instance;
}

void AIStoryteller::destroyInstance() {
	delete instance;
	instance = nullptr;
}

void AIStoryteller::update(Player* player, int moves) {
	totalMoves = moves;
	movesSinceLastEvent++;
	
	if (clueSpawnCooldown > 0) clueSpawnCooldown--;
	
	// CRITERION 1: Track player health ratio
	if (player) {
		playerHealthRatio = static_cast<float>(player->getHealth()) / player->getMaxHealth();
		playerLevel = player->getLevel();
	}
	
	// CRITERION 2: Time-based progression
	gameTimeSeconds = totalMoves * 5;
	
	calculateTension();
	updateDifficulty();
	
	if (eventCooldown > 0) {
		eventCooldown--;
	}
}

void AIStoryteller::incrementTime(int seconds) {
	gameTimeSeconds += seconds;
}

void AIStoryteller::initializeClues(const std::vector<int>& allClueIDs) {
	availableClueIDs = allClueIDs;
	spawnedClueIDs.clear();
}

void AIStoryteller::calculateTension() {
	float timeTension = (gameTimeSeconds / 600.0f);
	if (timeTension > 1.0f) timeTension = 1.0f;
	
	float healthTension = 1.0f - playerHealthRatio;
	
	tensionLevel = (timeTension * 0.6f) + (healthTension * 0.4f);
	if (tensionLevel > 1.0f) tensionLevel = 1.0f;
	if (tensionLevel < 0.0f) tensionLevel = 0.0f;
}

void AIStoryteller::updateDifficulty() {
	difficultyMultiplier = 1.0f + (gameTimeSeconds / 300.0f) * 0.5f;
	
	if (playerHealthRatio < 0.3f) {
		difficultyMultiplier *= 0.6f;
		std::cout << "\n  [AI STORYTELLER] Player struggling - reducing difficulty\n";
	}
	else if (playerHealthRatio < 0.5f) {
		difficultyMultiplier *= 0.8f;
	}
	
	if (playerLevel >= 5) {
		difficultyMultiplier *= 1.2f;
	}
}

// ============================================================================
// NEW: SPAWN DECISION METHODS (Replace hardcoded percentages)
// ============================================================================

bool AIStoryteller::shouldSpawnZombie() {
	// Base 15% chance, adjusted by player state
	int baseChance = 15;

	// High HP players face more combat
	if (playerHealthRatio > 0.7f) {
		baseChance += 15;  // 30% when healthy
		std::cout << "  [AI] High HP detected - increasing zombie spawn chance to " << baseChance << "%\n";
	}
	else if (playerHealthRatio < 0.3f) {
		baseChance -= 10;  // 5% when critical
		std::cout << "  [AI] Low HP detected - reducing zombie spawn chance to " << baseChance << "%\n";
	}

	// More combat in late game
	if (gameTimeSeconds > 300) {
		baseChance += 10;
		std::cout << "  [AI] Late game detected - increasing difficulty\n";
	}

	return (rand() % 100) < baseChance;
}

bool AIStoryteller::shouldSpawnLoot() {
	// Base 45% chance, adjusted by player state
	int baseChance = 45;
	
	// Low HP players find more loot
	if (playerHealthRatio < 0.3f) {
		baseChance += 25;  // 70% when critical
		std::cout << "  [AI] Increasing loot chance (low HP)\n";
	}
	else if (playerHealthRatio < 0.5f) {
		baseChance += 15;  // 60% when hurt
	}
	
	return (rand() % 100) < baseChance;
}

bool AIStoryteller::shouldSpawnClue() {
	if (clueSpawnCooldown > 0) return false;

	// INCREASED: Base 50% chance (was 35%)
	int baseChance = 50;

	// Increase chance if player hasn't found clues in a while
	if (totalMoves > 20 && spawnedClueIDs.size() < availableClueIDs.size()) {
		baseChance += 20;  // +20% boost (was +15%)
		std::cout << "  [AI] Boosting clue chance - you haven't found many yet!\n";
	}

	// Ensure all clues eventually spawn
	if (spawnedClueIDs.size() < availableClueIDs.size()) {
		// Guarantee spawn after many moves
		if (totalMoves > 30 && (rand() % 100) < 70) {  // Earlier guarantee (was 50 moves, 60%)
			clueSpawnCooldown = 5;  // Shorter cooldown (was 10)
			std::cout << "  [AI] Guaranteeing clue spawn - ensuring story completion!\n";
			return true;
		}
	}

	if ((rand() % 100) < baseChance) {
		clueSpawnCooldown = 5;  // Shorter cooldown (was 10)
		return true;
	}

	return false;
}

// ============================================================================
// NEW: ZOMBIE DIFFICULTY SCALING
// ============================================================================

int AIStoryteller::getZombieWaveCount() {
	// High HP: More waves
	if (playerHealthRatio > 0.7f) {
		int waves = 2 + (rand() % 2);  // 2-3 waves
		std::cout << "  [AI] High HP - spawning " << waves << " waves\n";
		return waves;
	}
	// Low HP: Fewer waves
	else if (playerHealthRatio < 0.3f) {
		std::cout << "  [AI] Low HP - spawning only 1 wave (mercy)\n";
		return 1;  // Just 1 wave
	}
	// Medium HP: Normal
	int waves = 1 + (rand() % 2);  // 1-2 waves
	return waves;
}

std::string AIStoryteller::getZombieType() {
	// High HP (>70%): Tougher zombies
	if (playerHealthRatio > 0.7f) {
		int type = rand() % 100;
		if (type < 30) {
			std::cout << "  [AI] Spawning Tank (you're doing well!)\n";
			return "Tank";      // 30% Tank
		}
		if (type < 55) return "Smoker";    // 25% Smoker
		if (type < 80) return "Spitter";   // 25% Spitter
		return "Boomer";                    // 20% Boomer
	}
	// Low HP (<30%): Easier zombies
	else if (playerHealthRatio < 0.3f) {
		int type = rand() % 100;
		if (type < 50) {
			std::cout << "  [AI] Spawning weaker zombies (you need a break)\n";
			return "Boomer";    // 50% Boomer (easiest)
		}
		if (type < 80) return "Spitter";   // 30% Spitter
		return "Smoker";                    // 20% Smoker
	}
	// Medium HP: Mixed
	else {
		int type = rand() % 100;
		if (type < 40) return "Boomer";
		if (type < 65) return "Spitter";
		if (type < 85) return "Smoker";
		return "Tank";
	}
}

// ============================================================================
// NEW: RANDOM LOOT GENERATION
// ============================================================================

std::string AIStoryteller::generateRandomLoot() {
	int lootType = rand() % 100;
	
	// Low HP: More medical items
	if (playerHealthRatio < 0.5f) {
		if (lootType < 60) return "medkit";      // 60% medical
		if (lootType < 75) return "bandages";    // 15% bandages
		if (lootType < 85) return "material";    // 10% materials
		return "weapon";                          // 5% weapons
	}
	// High HP: More weapons/materials
	else {
		if (lootType < 30) return "medkit";      // 30% medical
		if (lootType < 50) return "weapon";      // 20% weapons
		if (lootType < 80) return "material";    // 30% materials
		return "bandages";                        // 20% bandages
	}
}

int AIStoryteller::getLootQuantity() {
	// Low HP: More loot
	if (playerHealthRatio < 0.3f) {
		return 2 + (rand() % 2);  // 2-3 items
	}
	return 1 + (rand() % 2);  // 1-2 items
}

// ============================================================================
// NEW: RANDOM CLUE GENERATION
// ============================================================================

int AIStoryteller::generateRandomClue() {
	// Find unspawned clues
	std::vector<int> unspawnedClues;
	for (int clueID : availableClueIDs) {
		bool found = false;
		for (int spawnedID : spawnedClueIDs) {
			if (spawnedID == clueID) {
				found = true;
				break;
			}
		}
		if (!found) {
			unspawnedClues.push_back(clueID);
		}
	}
	
	if (unspawnedClues.empty()) {
		return -1;  // All clues spawned
	}
	
	// Pick random unspawned clue
	int clueID = unspawnedClues[rand() % unspawnedClues.size()];
	spawnedClueIDs.push_back(clueID);
	
	std::cout << "  [AI] Spawning random clue encounter!\n";
	return clueID;
}

// ============================================================================
// ASPECT 1: ZOMBIE SPAWN INFLUENCE
// ============================================================================

int AIStoryteller::adjustZombieCount(int baseCount) {
	if (playerHealthRatio < 0.3f) {
		baseCount -= 2;
		std::cout << "  [AI] Reducing zombie count (low HP)\n";
	}
	else if (playerHealthRatio < 0.5f) {
		baseCount -= 1;
	}
	
	if (gameTimeSeconds > 600) {
		baseCount += 2;
		std::cout << "  [AI] Increasing zombie count (late game)\n";
	}
	else if (gameTimeSeconds > 300) {
		baseCount += 1;
	}
	
	if (baseCount < 2) baseCount = 2;
	if (baseCount > 8) baseCount = 8;
	
	return baseCount;
}

float AIStoryteller::getZombieDifficultyModifier() {
	return difficultyMultiplier;
}

bool AIStoryteller::shouldSpawnSpecialZombie() {
	int chance = static_cast<int>(tensionLevel * 30) + (gameTimeSeconds / 60);
	return (rand() % 100) < chance;
}

// ============================================================================
// ASPECT 2: LOOT DROP INFLUENCE
// ============================================================================

float AIStoryteller::getLootQualityModifier() {
	if (playerHealthRatio < 0.3f) {
		std::cout << "  [AI] Improving loot quality (low HP)\n";
		return 1.8f;
	}
	else if (playerHealthRatio < 0.5f) {
		return 1.4f;
	}
	
	if (gameTimeSeconds > 600) {
		return 1.2f;
	}
	
	return 1.0f;
}

int AIStoryteller::getBonusLootChance() {
	int bonus = 0;
	
	if (playerHealthRatio < 0.5f) {
		bonus += 25;
	}
	
	if (gameTimeSeconds > 300) {
		bonus += 10;
	}
	
	return bonus;
}

bool AIStoryteller::shouldGrantBonusLoot() {
	if (playerHealthRatio < 0.3f && (rand() % 100) < 40) {
		std::cout << "  [AI] Granting bonus loot (struggling player)\n";
		return true;
	}
	return false;
}

// ============================================================================
// ASPECT 3: RANDOM EVENT INFLUENCE
// ============================================================================

bool AIStoryteller::shouldTriggerEvent() {
	if (eventCooldown > 0) return false;
	if (movesSinceLastEvent < 10) return false;
	
	int baseChance = 5;
	
	if (tensionLevel > 0.7f) {
		baseChance += 15;
	}
	
	if (gameTimeSeconds > 300 && movesSinceLastEvent > 20) {
		baseChance += 10;
	}
	
	return (rand() % 100) < baseChance;
}

std::string AIStoryteller::generateRandomEvent() {
	movesSinceLastEvent = 0;
	eventCooldown = 15;

	// Event selection based on player state
	if (playerHealthRatio < 0.3f) {
		int event = rand() % 4;  // Added STORY_CLUE option
		if (event == 0) return "SUPPLY_DROP";
		if (event == 1) return "SAFE_ZONE";
		if (event == 2) return "STORY_CLUE";  // NEW: Give struggling player story clues
		return "MEDICAL_CACHE";
	}
	else if (tensionLevel > 0.7f) {
		int event = rand() % 4;
		if (event == 0) return "HORDE_INCOMING";
		if (event == 1) return "ENVIRONMENTAL_HAZARD";
		if (event == 2) return "TOXIC_FOG";
		return "ELITE_ZOMBIE";
	}
	else {
		int event = rand() % 7;  // Added STORY_CLUE option
		if (event == 0) return "SUPPLY_DROP";
		if (event == 1) return "WANDERING_TRADER";
		if (event == 2) return "ZOMBIE_PATROL";
		if (event == 3) return "MYSTERIOUS_SOUND";
		if (event == 4) return "ABANDONED_CAMP";
		if (event == 5) return "STORY_CLUE";  // NEW: Random clue encounter
		return "TOXIC_FOG";
	}
}

void AIStoryteller::handleEvent(const std::string& eventType) {
	std::cout << "\n" << std::string(60, '=') << "\n";
	std::cout << "  [RANDOM EVENT] " << eventType << "\n";
	std::cout << std::string(60, '=') << "\n\n";
	
	if (eventType == "SUPPLY_DROP") {
		std::cout << "  A supply crate has been spotted nearby!\n";
		std::cout << "  You find extra medical supplies and ammunition.\n";
	}
	else if (eventType == "SAFE_ZONE") {
		std::cout << "  You discover a fortified safe zone!\n";
		std::cout << "  You can rest here safely.\n";
	}
	else if (eventType == "MEDICAL_CACHE") {
		std::cout << "  You stumble upon an abandoned medical cache!\n";
		std::cout << "  Bandages and medicine are yours for the taking.\n";
	}
	else if (eventType == "HORDE_INCOMING") {
		std::cout << "  WARNING: A zombie horde is approaching!\n";
		std::cout << "  Prepare for intense combat ahead.\n";
	}
	else if (eventType == "ENVIRONMENTAL_HAZARD") {
		std::cout << "  DANGER: The area is unstable!\n";
		std::cout << "  Proceed with caution.\n";
	}
	else if (eventType == "TOXIC_FOG") {
		std::cout << "  TOXIC FOG is rolling in!\n";
		std::cout << "  The air becomes thick and dangerous.\n";
		std::cout << "  You take 10 damage from the toxic fumes!\n";
	}
	else if (eventType == "ELITE_ZOMBIE") {
		std::cout << "  A powerful elite zombie has appeared!\n";
		std::cout << "  This will be a tough fight.\n";
	}
	else if (eventType == "WANDERING_TRADER") {
		std::cout << "  A mysterious trader offers supplies...\n";
		std::cout << "  But at what cost?\n";
	}
	else if (eventType == "ZOMBIE_PATROL") {
		std::cout << "  You spot a zombie patrol in the distance.\n";
		std::cout << "  You can avoid them or engage.\n";
	}
	else if (eventType == "MYSTERIOUS_SOUND") {
		std::cout << "  You hear strange sounds nearby...\n";
		std::cout << "  Something is out there.\n";
	}
	else if (eventType == "ABANDONED_CAMP") {
		std::cout << "  You find an abandoned survivor camp.\n";
		std::cout << "  There might be useful supplies here.\n";
	}
	else if (eventType == "STORY_CLUE") {
		std::cout << "  You discover an important document!\n";
		std::cout << "  This could shed light on what happened...\n";
	}

	std::cout << "\n  Press ENTER to continue...";
	std::cin.get();
}

std::string AIStoryteller::getStorytellerStatus() {
	std::string status = "\n  [AI STORYTELLER STATUS]\n";
	status += "  Game Time: " + std::to_string(gameTimeSeconds) + "s\n";
	status += "  Player Health: " + std::to_string(static_cast<int>(playerHealthRatio * 100)) + "%\n";
	status += "  Tension Level: " + std::to_string(static_cast<int>(tensionLevel * 100)) + "%\n";
	status += "  Difficulty: " + std::to_string(difficultyMultiplier) + "x\n";
	status += "  Clues Spawned: " + std::to_string(spawnedClueIDs.size()) + "/" + std::to_string(availableClueIDs.size()) + "\n";
	return status;
}

AIStoryteller::~AIStoryteller() {
}
