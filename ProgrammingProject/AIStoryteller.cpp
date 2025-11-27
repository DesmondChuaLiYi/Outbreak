#include "AIStoryteller.h"
#include "Player.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

AIStoryteller* AIStoryteller::instance = nullptr;

AIStoryteller::AIStoryteller() 
	: gameTimeSeconds(0), totalMoves(0), playerHealthRatio(1.0f), 
	  playerLevel(1), difficultyMultiplier(1.0f), tensionLevel(0.0f),
	  movesSinceLastEvent(0), eventCooldown(0) {
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
	
	// CRITERION 1: Track player health ratio
	if (player) {
		playerHealthRatio = static_cast<float>(player->getHealth()) / player->getMaxHealth();
		playerLevel = player->getLevel();
	}
	
	// CRITERION 2: Time-based progression (moves as time proxy)
	gameTimeSeconds = totalMoves * 5;  // Assume ~5 seconds per move
	
	calculateTension();
	updateDifficulty();
	
	if (eventCooldown > 0) {
		eventCooldown--;
	}
}

void AIStoryteller::incrementTime(int seconds) {
	gameTimeSeconds += seconds;
}

void AIStoryteller::calculateTension() {
	// Tension increases with time and decreases with good health
	float timeTension = (gameTimeSeconds / 600.0f);  // Increases over 10 minutes
	if (timeTension > 1.0f) timeTension = 1.0f;
	
	float healthTension = 1.0f - playerHealthRatio;  // Higher when low HP
	
	// Combine factors
	tensionLevel = (timeTension * 0.6f) + (healthTension * 0.4f);
	if (tensionLevel > 1.0f) tensionLevel = 1.0f;
	if (tensionLevel < 0.0f) tensionLevel = 0.0f;
}

void AIStoryteller::updateDifficulty() {
	// Base difficulty increases with time
	difficultyMultiplier = 1.0f + (gameTimeSeconds / 300.0f) * 0.5f;  // +50% every 5 min
	
	// Reduce difficulty if player is struggling (low HP)
	if (playerHealthRatio < 0.3f) {
		difficultyMultiplier *= 0.6f;  // 40% easier when critical
		std::cout << "\n  [AI STORYTELLER] Player struggling - reducing difficulty\n";
	}
	else if (playerHealthRatio < 0.5f) {
		difficultyMultiplier *= 0.8f;  // 20% easier when hurt
	}
	
	// Increase difficulty for high-level players
	if (playerLevel >= 5) {
		difficultyMultiplier *= 1.2f;
	}
}

// ============================================================================
// ASPECT 1: ZOMBIE SPAWN INFLUENCE
// ============================================================================

int AIStoryteller::adjustZombieCount(int baseCount) {
	// CRITERION 1: Health-based adjustment
	if (playerHealthRatio < 0.3f) {
		baseCount -= 2;  // Fewer zombies when critical
		std::cout << "  [AI] Reducing zombie count (low HP)\n";
	}
	else if (playerHealthRatio < 0.5f) {
		baseCount -= 1;  // Slightly fewer when hurt
	}
	
	// CRITERION 2: Time-based adjustment
	if (gameTimeSeconds > 600) {  // After 10 minutes
		baseCount += 2;  // More zombies late game
		std::cout << "  [AI] Increasing zombie count (late game)\n";
	}
	else if (gameTimeSeconds > 300) {  // After 5 minutes
		baseCount += 1;
	}
	
	// Ensure minimum
	if (baseCount < 2) baseCount = 2;
	if (baseCount > 8) baseCount = 8;
	
	return baseCount;
}

float AIStoryteller::getZombieDifficultyModifier() {
	return difficultyMultiplier;
}

bool AIStoryteller::shouldSpawnSpecialZombie() {
	// Higher chance based on tension and time
	int chance = static_cast<int>(tensionLevel * 30) + (gameTimeSeconds / 60);
	return (rand() % 100) < chance;
}

// ============================================================================
// ASPECT 2: LOOT DROP INFLUENCE
// ============================================================================

float AIStoryteller::getLootQualityModifier() {
	// CRITERION 1: Better loot when player is struggling
	if (playerHealthRatio < 0.3f) {
		std::cout << "  [AI] Improving loot quality (low HP)\n";
		return 1.8f;  // 80% better loot
	}
	else if (playerHealthRatio < 0.5f) {
		return 1.4f;  // 40% better loot
	}
	
	// CRITERION 2: Slightly better loot over time
	if (gameTimeSeconds > 600) {
		return 1.2f;
	}
	
	return 1.0f;
}

int AIStoryteller::getBonusLootChance() {
	int bonus = 0;
	
	// CRITERION 1: Bonus chance when hurt
	if (playerHealthRatio < 0.5f) {
		bonus += 25;  // +25% loot chance
	}
	
	// CRITERION 2: Small bonus over time
	if (gameTimeSeconds > 300) {
		bonus += 10;
	}
	
	return bonus;
}

bool AIStoryteller::shouldGrantBonusLoot() {
	// Extra loot when player needs help
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
	if (movesSinceLastEvent < 10) return false;  // Minimum 10 moves between events
	
	// CRITERION 1 & 2: Event chance based on tension and time
	int baseChance = 5;  // 5% base chance per move
	
	if (tensionLevel > 0.7f) {
		baseChance += 15;  // More events at high tension
	}
	
	if (gameTimeSeconds > 300 && movesSinceLastEvent > 20) {
		baseChance += 10;  // More events in late game
	}
	
	return (rand() % 100) < baseChance;
}

std::string AIStoryteller::generateRandomEvent() {
	movesSinceLastEvent = 0;
	eventCooldown = 15;  // Cooldown before next event
	
	// Event selection based on player state
	if (playerHealthRatio < 0.3f) {
		// Helpful events when struggling
		int event = rand() % 3;
		if (event == 0) return "SUPPLY_DROP";
		if (event == 1) return "SAFE_ZONE";
		return "MEDICAL_CACHE";
	}
	else if (tensionLevel > 0.7f) {
		// Challenging events at high tension
		int event = rand() % 3;
		if (event == 0) return "HORDE_INCOMING";
		if (event == 1) return "ENVIRONMENTAL_HAZARD";
		return "ELITE_ZOMBIE";
	}
	else {
		// Balanced events
		int event = rand() % 5;
		if (event == 0) return "SUPPLY_DROP";
		if (event == 1) return "WANDERING_TRADER";
		if (event == 2) return "ZOMBIE_PATROL";
		if (event == 3) return "MYSTERIOUS_SOUND";
		return "ABANDONED_CAMP";
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
	
	std::cout << "\n  Press ENTER to continue...";
	std::cin.get();
}

std::string AIStoryteller::getStorytellerStatus() {
	std::string status = "\n  [AI STORYTELLER STATUS]\n";
	status += "  Game Time: " + std::to_string(gameTimeSeconds) + "s\n";
	status += "  Player Health: " + std::to_string(static_cast<int>(playerHealthRatio * 100)) + "%\n";
	status += "  Tension Level: " + std::to_string(static_cast<int>(tensionLevel * 100)) + "%\n";
	status += "  Difficulty: " + std::to_string(difficultyMultiplier) + "x\n";
	return status;
}

AIStoryteller::~AIStoryteller() {
}
