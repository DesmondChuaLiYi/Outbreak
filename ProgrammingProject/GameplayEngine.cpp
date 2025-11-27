#include "GameplayEngine.h"
#include "AudioEngine.h"
#include "GameEngine.h"
#include "AIStoryteller.h"
#include "CommonInfected.h"
#include "Boomer.h"
#include "Spitter.h"
#include "Smoker.h"
#include "Tank.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <algorithm>
#include <thread>
#include <chrono>
#include <ctime>
#include <vector>

#ifdef _WIN32
#define CLEAR_SCREEN "cls"
#else
#define CLEAR_SCREEN "clear"
#endif

// ============================================================================
// SINGLETON INSTANCE
// ============================================================================

GameplayEngine* GameplayEngine::instance = nullptr;

// ============================================================================
// CONSTRUCTORS & DESTRUCTORS
// ============================================================================

GameplayEngine::GameplayEngine()
	: currentPlayer(nullptr), currentLocation(nullptr), journal(nullptr),
	currentWaveNumber(0), maxWavesPerLocation(1),
	movementSteps(0), stepsToNewLocation(0), inCombat(false), hasExploredNewArea(false) {
	srand(static_cast<unsigned int>(time(nullptr)));
}

GameplayEngine::~GameplayEngine() {
	// Clean up any remaining zombies in queue
	while (!currentWave.isEmpty()) {
		delete currentWave.dequeue();
	}
}

// ============================================================================
// SINGLETON PATTERN
// ============================================================================

GameplayEngine* GameplayEngine::getInstance() {
	if (instance == nullptr) {
		instance = new GameplayEngine();
	}
	return instance;
}

void GameplayEngine::destroyInstance() {
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}

// ============================================================================
// INITIALIZATION
// ============================================================================

void GameplayEngine::initialize(Player* player, Location* location, ClueJournal* journal) {
	currentPlayer = player;
	this->journal = journal;
	inCombat = false;
	currentWaveNumber = 0;
	movementSteps = 0;
	stepsToNewLocation = 0;
	hasExploredNewArea = false;

	// Clear combat action history (important when loading a saved game)
	while (!combatActionHistory.isEmpty()) {
		combatActionHistory.pop();
	}

	// Always set location to ensure proper initialization
	if (location != nullptr) {
		setCurrentLocation(location);
	}
}

void GameplayEngine::setCurrentLocation(Location* location) {
	if (location != nullptr) {
		currentLocation = location;
		stepsToNewLocation = 0;  // Reset exploration on new location
		movementSteps = 0;
		hasExploredNewArea = false;
		
		// Clear previous location's loot and clues
		currentLocationLoot.clear();
		currentLocationClues.clear();
		
		// ALWAYS repopulate - this is critical!
		populateLocationLoot();
		populateLocationClues();

		// Set max waves based on location's hazard
		if (location->getHazardDamage() > 5) {
			maxWavesPerLocation = 2;
		}
		else {
			maxWavesPerLocation = 1;
		}
	}
}

// ============================================================================
// UI & DISPLAY
// ============================================================================

void GameplayEngine::displayCurrentLocation() {
	system(CLEAR_SCREEN);
	std::cout << "\n";
	std::cout << std::string(80, '=') << "\n";
	std::cout << "  LOCATION: " << currentLocation->getName() << "\n";
	std::cout << std::string(80, '=') << "\n";
	std::cout << "\n";
	std::cout << "  " << currentLocation->getDescription() << "\n\n";

	if (currentLocation->hasHazard()) {
		std::cout << "  [!] HAZARD: " << currentLocation->hazardToString()
			<< " (" << currentLocation->getHazardDamage() << " HP/turn)\n\n";
	}

	// Enhanced Player HUD with ASCII bars
	std::cout << "  " << std::string(76, '-') << "\n";
	
	// Health bar
	int hp = currentPlayer->getHealth();
	int maxHP = currentPlayer->getMaxHealth();
	float hpPercent = (float)hp / maxHP;
	int barWidth = 20;
	int hpFilled = (int)(hpPercent * barWidth);
	
	std::cout << "  HP: [";
	for (int i = 0; i < barWidth; i++) {
		if (i < hpFilled) std::cout << "=";
		else std::cout << " ";
	}
	std::cout << "] " << hp << "/" << maxHP;
	
	// Hunger bar
	int hunger = currentPlayer->getHunger();
	int maxHunger = currentPlayer->getMaxHunger();
	float hungerPercent = (float)hunger / maxHunger;
	int hungerFilled = (int)(hungerPercent * barWidth);
	
	std::cout << "  Hunger: [";
	for (int i = 0; i < barWidth; i++) {
		if (i < hungerFilled) std::cout << "=";
		else std::cout << " ";
	}
	std::cout << "] " << hunger << "/" << maxHunger << "\n";
	
	// XP bar and Level
	int xp = currentPlayer->getExperience();
	int xpToNext = currentPlayer->getExperienceToNextLevel();
	float xpPercent = (float)xp / xpToNext;
	int xpFilled = (int)(xpPercent * barWidth);
	
	std::cout << "  XP: [";
	for (int i = 0; i < barWidth; i++) {
		if (i < xpFilled) std::cout << "=";
		else std::cout << " ";
	}
	std::cout << "] " << xp << "/" << xpToNext;
	std::cout << "  Level: " << currentPlayer->getLevel() << "  SP: " << currentPlayer->getSkillPoints() << "\n";
	
	// Weapon and Inventory
	std::cout << "  Weapon: " << currentPlayer->getEquippedWeapon() << " (+" << currentPlayer->getDamage() << " dmg)";
	std::cout << "    Inventory: " << currentPlayer->getInventorySize() << "/" << currentPlayer->getMaxInventorySpace() << "\n";
	
	// Exploration progress
	float progress = (float)stepsToNewLocation / 12.0f * 100.0f;
	if (progress > 100.0f) progress = 100.0f;
	int explorationFilled = (int)(progress / 100.0f * barWidth);
	
	std::cout << "  Exploration: [";
	for (int i = 0; i < barWidth; i++) {
		if (i < explorationFilled) std::cout << "=";
		else std::cout << " ";
	}
	std::cout << "] " << (int)progress << "% (" << stepsToNewLocation << "/12)\n";
	
	std::cout << "  " << std::string(76, '-') << "\n\n";

	displayLocationLayout();

	// Enhanced command help
	std::cout << "\n  " << std::string(76, '-') << "\n";
	std::cout << "  COMMANDS:\n";
	std::cout << "  Movement: go [left/right/up/down]";
	if (canTravelToNewLocation()) {
		std::cout << " | travel (ready!)";
	}
	std::cout << "\n";
	std::cout << "  Player:   status | inventory | skills | clues\n";
	std::cout << "  Actions:  craft | rest | save | menu\n";
	std::cout << "  " << std::string(76, '-') << "\n";
}

void GameplayEngine::displayLocationLayout() {
	std::cout << "  AREA MAP:\n";
	std::cout << "       [UP]\n";
	std::cout << "        |\n";
	std::cout << "  [LEFT]-+-[RIGHT]\n";
	std::cout << "        |\n";
	std::cout << "      [DOWN]\n\n";

	std::cout << "  LOOT:\n";
	bool hasLoot = false;
	for (const auto& loot : currentLocationLoot) {
		if (!loot.isPickedUp) {
			std::cout << "    - " << loot.item.getName() << " ["
				<< directionToString(loot.location) << "]\n";
			hasLoot = true;
		}
	}
	if (!hasLoot) std::cout << "    No visible loot.\n";

	std::cout << "\n  CLUES:\n";
	bool hasClues = false;
	for (const auto& clue : currentLocationClues) {
		if (!clue.collected) {
			std::cout << "    - " << clue.clueName << " ["
				<< directionToString(clue.location) << "]\n";
			hasClues = true;
		}
	}
	if (!hasClues) std::cout << "    No clues here.\n";
}

// ============================================================================
// LOOT & CLUE POPULATION
// ============================================================================

void GameplayEngine::populateLocationLoot() {
	currentLocationLoot.clear();
	if (currentLocation == nullptr) return;
	
	std::string locID = currentLocation->getID();

	if (locID == "loc_ruined_city") {
		Item bandage("loot_001", "Bandage", Item::Category::MEDICAL,
			"Restores 15 HP", 2, 3, true, true, 15, 0, 0, 0);
		currentLocationLoot.push_back(Loot("loot_001", bandage, Direction::LEFT));
		
		Item food("loot_002", "Canned Food", Item::Category::FOOD,
			"Restores 30 hunger", 3, 4, true, true, 0, 30, 0, 0);
		currentLocationLoot.push_back(Loot("loot_002", food, Direction::RIGHT));
		
		// NEW: More materials
		Item cloth("mat_cloth_city", "Cloth", Item::Category::MATERIAL,
			"Torn fabric for crafting", 3, 2, false, false, 0, 0, 0, 0);
		currentLocationLoot.push_back(Loot("mat_cloth_city", cloth, Direction::UP));
		
		Item wire("mat_wire_city", "Wire", Item::Category::MATERIAL,
			"Useful for repairs", 2, 1, false, false, 0, 0, 0, 0);
		currentLocationLoot.push_back(Loot("mat_wire_city", wire, Direction::DOWN));
	}
	else if (locID == "loc_industrial") {
		Item axe("loot_004", "Axe", Item::Category::WEAPON,
			"Heavy axe. 25 melee damage", 1, 8, false, true, 0, 0, 0, 25);
		currentLocationLoot.push_back(Loot("loot_004", axe, Direction::LEFT));
		
		// Add crafting materials
		Item cloth("mat_cloth", "Cloth", Item::Category::MATERIAL,
			"Used for crafting", 2, 2, false, false, 0, 0, 0, 0);
		currentLocationLoot.push_back(Loot("mat_cloth", cloth, Direction::DOWN));
		
		// NEW: More materials
		Item metalParts("mat_metal_parts", "Metal Parts", Item::Category::MATERIAL,
			"Scrap metal pieces", 4, 3, false, false, 0, 0, 0, 0);
		currentLocationLoot.push_back(Loot("mat_metal_parts", metalParts, Direction::UP));
		
		Item wire("mat_wire_industrial", "Wire", Item::Category::MATERIAL,
			"Industrial wire", 3, 1, false, false, 0, 0, 0, 0);
		currentLocationLoot.push_back(Loot("mat_wire_industrial", wire, Direction::RIGHT));
	}
	else if (locID == "loc_hollow_woods") {
		Item rifle("loot_rifle", "Hunting Rifle", Item::Category::WEAPON,
			"Powerful rifle. 40 damage", 1, 10, false, true, 0, 0, 0, 40);
		currentLocationLoot.push_back(Loot("loot_rifle", rifle, Direction::UP));
		
		// Add crafting materials
		Item herbs("mat_herbs", "Herbs", Item::Category::MATERIAL,
			"Used for potions", 5, 1, false, false, 0, 0, 0, 0);
		currentLocationLoot.push_back(Loot("mat_herbs", herbs, Direction::LEFT));
		
		// NEW: More materials
		Item food("mat_food_woods", "Food Rations", Item::Category::FOOD,
			"Preserved food", 2, 3, true, true, 0, 20, 0, 0);
		currentLocationLoot.push_back(Loot("mat_food_woods", food, Direction::RIGHT));
		
		Item bandages("mat_bandages_woods", "Bandages", Item::Category::MATERIAL,
			"Medical supplies", 3, 2, false, false, 0, 0, 0, 0);
		currentLocationLoot.push_back(Loot("mat_bandages_woods", bandages, Direction::DOWN));
	}
	else if (locID == "loc_cemetery") {
		Item bandage("loot_cem", "Bandage", Item::Category::MEDICAL,
			"Medical supplies", 3, 3, true, true, 15, 0, 0, 0);
		currentLocationLoot.push_back(Loot("loot_cem", bandage, Direction::UP));
		
		Item cloth("mat_cloth2", "Cloth", Item::Category::MATERIAL,
			"Crafting material", 4, 2, false, false, 0, 0, 0, 0);
		currentLocationLoot.push_back(Loot("mat_cloth2", cloth, Direction::RIGHT));
		
		// NEW: More materials
		Item bandages("mat_bandages_cem", "Bandages", Item::Category::MATERIAL,
			"First aid supplies", 4, 2, false, false, 0, 0, 0, 0);
		currentLocationLoot.push_back(Loot("mat_bandages_cem", bandages, Direction::LEFT));
	}
	else if (locID == "loc_old_mill") {
		// NEW: Add loot to Old Mill
		Item metalParts("mat_metal_mill", "Metal Parts", Item::Category::MATERIAL,
			"Rusty metal pieces", 3, 3, false, false, 0, 0, 0, 0);
		currentLocationLoot.push_back(Loot("mat_metal_mill", metalParts, Direction::LEFT));
		
		Item food("mat_food_mill", "Food Rations", Item::Category::FOOD,
			"Old canned food", 2, 3, true, true, 0, 25, 0, 0);
		currentLocationLoot.push_back(Loot("mat_food_mill", food, Direction::RIGHT));
	}
	else if (locID == "loc_canal") {
		Item water("mat_water", "Water", Item::Category::MATERIAL,
			"Can be used for potions", 3, 2, false, false, 0, 0, 0, 0);
		currentLocationLoot.push_back(Loot("mat_water", water, Direction::DOWN));
		
		// NEW: More materials
		Item chemicals("mat_chemicals_canal", "Chemical Supplies", Item::Category::MATERIAL,
			"Industrial chemicals", 2, 2, false, false, 0, 0, 0, 0);
		currentLocationLoot.push_back(Loot("mat_chemicals_canal", chemicals, Direction::UP));
	}
	else if (locID == "loc_pump_station") {
		Item antibiotics("mat_antibiotics", "Antibiotics", Item::Category::MATERIAL,
			"Medical crafting material", 2, 2, false, false, 0, 0, 0, 0);
		currentLocationLoot.push_back(Loot("mat_antibiotics", antibiotics, Direction::LEFT));
		
		Item scrap("mat_scrap", "Scrap Metal", Item::Category::MATERIAL,
			"For weapon crafting", 5, 1, false, false, 0, 0, 0, 0);
		currentLocationLoot.push_back(Loot("mat_scrap", scrap, Direction::RIGHT));
		
		// NEW: More materials
		Item wire("mat_wire_pump", "Wire", Item::Category::MATERIAL,
			"Electrical wire", 4, 1, false, false, 0, 0, 0, 0);
		currentLocationLoot.push_back(Loot("mat_wire_pump", wire, Direction::UP));
		
		Item chemicals("mat_chemicals_pump", "Chemical Supplies", Item::Category::MATERIAL,
			"Lab chemicals", 2, 2, false, false, 0, 0, 0, 0);
		currentLocationLoot.push_back(Loot("mat_chemicals_pump", chemicals, Direction::DOWN));
	}
	
	// CRITICAL FIX: Mark loot as picked up if it's in the pickedUpLootIDs list
	for (auto& loot : currentLocationLoot) {
		for (const auto& pickedUpID : pickedUpLootIDs) {
			if (loot.lootID == pickedUpID) {
				loot.isPickedUp = true;
				break;
			}
		}
	}
}

void GameplayEngine::populateLocationClues() {
	currentLocationClues.clear();
	if (currentLocation == nullptr) return;
	
	std::string locID = currentLocation->getID();

	if (locID == "loc_ruined_city") {
		currentLocationClues.push_back(ClueLocation(1, "City Engineer's Journal", Direction::RIGHT));
		currentLocationClues.push_back(ClueLocation(2, "Graffiti: THE WATER KILLED US", Direction::DOWN));
	}
	else if (locID == "loc_industrial") {
		currentLocationClues.push_back(ClueLocation(6, "Industrial Memo", Direction::LEFT));
		currentLocationClues.push_back(ClueLocation(8, "Warehouse Blueprint", Direction::UP));
	}
	else if (locID == "loc_hollow_woods") {
		currentLocationClues.push_back(ClueLocation(11, "Hiker's Journal", Direction::UP));
		currentLocationClues.push_back(ClueLocation(12, "Hidden Campsite Ledger", Direction::LEFT));
	}
	else if (locID == "loc_old_mill") {
		currentLocationClues.push_back(ClueLocation(16, "The Butcher's Family Photo", Direction::UP));
	}
	else if (locID == "loc_cemetery") {
		currentLocationClues.push_back(ClueLocation(20, "Gravedigger's Diary", Direction::LEFT));
		currentLocationClues.push_back(ClueLocation(22, "Cemetery Gate Key", Direction::DOWN));
	}
	else if (locID == "loc_canal") {
		currentLocationClues.push_back(ClueLocation(25, "Government Containment Report", Direction::RIGHT));
	}
	else if (locID == "loc_pump_station") {
		currentLocationClues.push_back(ClueLocation(26, "Tony's Workstation Note", Direction::UP));
	}
	
	// CRITICAL FIX: Mark clues as collected if they're in the journal's collected list
	if (journal != nullptr) {
		std::vector<int> collectedIDs = journal->getCollectedClueIDs();
		for (auto& clue : currentLocationClues) {
			for (int collectedID : collectedIDs) {
				if (clue.clueID == collectedID) {
					clue.collected = true;
					break;
				}
			}
		}
	}
}

// ============================================================================
// MOVEMENT & EXPLORATION
// ============================================================================

void GameplayEngine::moveInDirection(Direction direction) {
	system(CLEAR_SCREEN);
	
	// Decrease hunger per movement
	int currentHunger = currentPlayer->getHunger();
	currentPlayer->setHunger(currentHunger - 2);
	
	// Hunger status effects
	std::string hungerStatus = "";
	if (currentHunger <= 10) {
		hungerStatus = " [STARVING]";
		// Health drain when starving
		currentPlayer->takeDamage(1);
		std::cout << "\n  [WARNING] You're starving! (-1 HP)\n";
	}
	else if (currentHunger <= 30) {
		hungerStatus = " [HUNGRY]";
	}
	
	std::cout << "\n  You explore " << directionToString(direction) << "..." << hungerStatus << "\n\n";
	
	// Increment exploration
	movementSteps++;
	stepsToNewLocation++;
	
	// AI STORYTELLER: Update with current player state
	AIStoryteller* aiStoryteller = AIStoryteller::getInstance();
	aiStoryteller->update(currentPlayer, movementSteps);
	
	// AI STORYTELLER INFLUENCE #3: Random events based on player state
	if (aiStoryteller->shouldTriggerEvent()) {
		std::string event = aiStoryteller->generateRandomEvent();
		aiStoryteller->handleEvent(event);
		
		// Apply event effects
		if (event == "SUPPLY_DROP" || event == "MEDICAL_CACHE") {
			// Healing event - restore some HP
			int healAmount = (event == "SUPPLY_DROP") ? 50 : 20;
			int newHP = currentPlayer->getHealth() + healAmount;
			if (newHP > currentPlayer->getMaxHealth()) newHP = currentPlayer->getMaxHealth();
			currentPlayer->setHealth(newHP);
			std::cout << "  [HEALED] Restored " << healAmount << " HP!\n";
		}
		else if (event == "HORDE_INCOMING") {
			startCombat();
			return;  // Skip normal exploration
		}
		else if (event == "SAFE_ZONE") {
			int healAmount = 30;
			int newHP = currentPlayer->getHealth() + healAmount;
			if (newHP > currentPlayer->getMaxHealth()) newHP = currentPlayer->getMaxHealth();
			currentPlayer->setHealth(newHP);
		}
	}
	
	// Determine ONE event type (better pacing - no simultaneous events)
	int eventRoll = rand() % 100;
	bool eventOccurred = false;
	
	// Apply scavenging skill bonus to loot chance
	float scavengeBonus = currentPlayer->getSkillTree().getTotalScavengeBonus();
	int lootChance = 45; // Base 45%
	if (scavengeBonus > 0) {
		lootChance = (int)(45 * (1.0f + scavengeBonus));
		if (lootChance > 70) lootChance = 70; // Cap at 70%
	}
	
	// Priority: Loot (45% + bonus) > Clue (35%) > Combat (15%) > Hazard (5%)
	if (eventRoll < lootChance && !eventOccurred) {
		// Loot event (increased from 30% to 45%, with scavenge bonus)
		checkForLoot(direction);
		eventOccurred = true;
		
		// AI STORYTELLER INFLUENCE #2: Bonus healing when player is struggling
		AIStoryteller* ai = AIStoryteller::getInstance();
		if (ai->shouldGrantBonusLoot()) {
			int bonusHeal = 20;
			int newHP = currentPlayer->getHealth() + bonusHeal;
			if (newHP > currentPlayer->getMaxHealth()) newHP = currentPlayer->getMaxHealth();
			currentPlayer->setHealth(newHP);
			std::cout << "  [AI BONUS] Found emergency supplies! Restored " << bonusHeal << " HP!\n";
		}
		
		// Scavenger bonus: chance for extra loot!
		if (scavengeBonus > 0 && rand() % 100 < (int)(scavengeBonus * 100)) {
			std::cout << "  [SCAVENGER] You spot extra loot nearby!\n";
			// Find another loot in a different direction
			Direction extraDir = static_cast<Direction>((static_cast<int>(direction) + 1) % 4);
			checkForLoot(extraDir);
		}
	}
	else if (eventRoll < 80 && !eventOccurred) {
		// Clue event (increased from 25% to 35%, cumulative 45+35=80)
		checkForClue(direction);
		eventOccurred = true;
	}
	else if (eventRoll < 95 && currentLocation->getZombieCount() > 0 && !eventOccurred) {
		// Combat event (reduced from 30% to 15%)
		std::cout << "  [!] Zombies detected!\n\n";
		AudioEngine::getInstance()->playCombatAttackSound();
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		startCombat();
		eventOccurred = true;
	}
	else if (eventRoll < 100 && !eventOccurred) {
		// Hazard event (5%)
		checkForHazard();
		eventOccurred = true;
	}
	
	// Nothing found (5% chance or if no events available)
	if (!eventOccurred) {
		std::cout << "  The area is quiet. Nothing of interest here.\n\n";
	}
	
	// Show exploration progress
	float progress = (float)stepsToNewLocation / 12.0f * 100.0f;
	if (progress > 100.0f) progress = 100.0f;
	
	std::cout << "\n  Area Explored: " << (int)progress << "%";
	int barWidth = 20;
	int filled = (int)(progress / 100.0f * barWidth);
	std::cout << " [";
	for (int i = 0; i < barWidth; i++) {
		if (i < filled) std::cout << "=";
		else std::cout << " ";
	}
	std::cout << "]\n";
	
	// Check if ready for new location (changed from 15 to 12)
	if (stepsToNewLocation >= 12 && !hasExploredNewArea) {
		std::cout << "\n  [!] Area fully explored!";
		std::cout << "\n  [!] You can now travel to a new location (type 'travel').\n";
		hasExploredNewArea = true;
	}
	
	// Hunger warning
	if (currentPlayer->getHunger() <= 30 && currentPlayer->getHunger() > 10) {
		std::cout << "\n  [!] You're getting hungry. Find food soon!\n";
	}
	
	std::cout << "\n  Press ENTER...";
	std::cin.get();
}

void GameplayEngine::checkForLoot(Direction direction) {
	for (auto& loot : currentLocationLoot) {
		if (loot.location == direction && !loot.isPickedUp) {
			std::cout << "  [LOOT] Found: " << loot.item.getName() << "!\n";
			std::cout << "  Pick up? (y/n): ";

			char choice;
			std::cin >> choice;
			std::cin.ignore();

			if (choice == 'y' || choice == 'Y') {
				if (currentPlayer->getInventorySize() + loot.item.getInventorySpace()
					<= currentPlayer->getMaxInventorySpace()) {
					currentPlayer->addItem(loot.item);
					loot.isPickedUp = true;
					addPickedUpLootID(loot.lootID); // Record this loot was picked up
					AudioEngine::getInstance()->playLootPickupSound();
					std::cout << "  [SUCCESS] Added!\n\n";
				}
				else {
					std::cout << "  [ERROR] Inventory full!\n\n";
				}
			}
			break;
		}
	}
}

void GameplayEngine::checkForClue(Direction direction) {
	for (auto& clue : currentLocationClues) {
		if (clue.location == direction && !clue.collected) {
			std::cout << "  [CLUE] " << clue.clueName << "!\n\n";
			Clue* actualClue = journal->getClue(clue.clueID);
			if (actualClue != nullptr) {
				journal->collectClue(clue.clueID);
				clue.collected = true;
			}
			break;
		}
	}
}

void GameplayEngine::checkForHazard() {
	if (currentLocation->getHazardDamage() > 0) {
		int damage = currentLocation->getHazardDamage();
		std::cout << "  [HAZARD] " << currentLocation->hazardToString() << "!\n";
		std::cout << "  You take " << damage << " damage!\n\n";
		currentPlayer->takeDamage(damage);
		AudioEngine::getInstance()->playEnvironmentalHazardSound();
	}
}

void GameplayEngine::checkForLocationTransition() {
	// Placeholder for future implementation
}

// ============================================================================
// DISPLAY OPTIONS
// ============================================================================

void GameplayEngine::displayLootOptions() {
	system(CLEAR_SCREEN);
	std::cout << "\n  AVAILABLE LOOT:\n\n";

	int count = 0;
	for (const auto& loot : currentLocationLoot) {
		if (!loot.isPickedUp) {
			count++;
			std::cout << "    - " << loot.item.getName()
				<< " (" << directionToString(loot.location) << ")\n";
		}
	}

	if (count == 0) {
		std::cout << "    No loot here.\n";
	}

	std::cout << "\n  Press ENTER...";
	std::cin.get();
}

// ============================================================================
// LOOT & CLUE INTERACTION
// ============================================================================

void GameplayEngine::interactWithLoot(Direction direction) {
	checkForLoot(direction);
}

void GameplayEngine::interactWithClue(int clueIndex) {
	if (clueIndex >= 0 && clueIndex < static_cast<int>(currentLocationClues.size())) {
		auto& clue = currentLocationClues[clueIndex];
		if (!clue.collected) {
			clue.collected = true;
			journal->collectClue(clue.clueID);
		}
	}
}

// ============================================================================
// COMBAT SYSTEM
// ============================================================================

void GameplayEngine::startCombat() {
	inCombat = true;
	currentWaveNumber = 0;

	system(CLEAR_SCREEN);
	std::cout << "\n" << std::string(80, '=') << "\n";
	std::cout << "  COMBAT INITIATED!\n";
	std::cout << std::string(80, '=') << "\n\n";

	spawnZombieWave();
	CombatResult result = conductCombat();

	if (result.playerWon) {
		std::cout << "  [REWARD] Victory! You gained experience.\n";
	}
}

void GameplayEngine::spawnZombieWave() {
	currentWaveNumber++;
	std::cout << "  [WAVE " << currentWaveNumber << "]\n\n";

	while (!currentWave.isEmpty()) {
		delete currentWave.dequeue();
	}

	int zombieCount = 3 + (rand() % 4); // 3-6 zombies per wave
	
	// AI STORYTELLER INFLUENCE #1: Adjust zombie count based on player state
	AIStoryteller* ai = AIStoryteller::getInstance();
	zombieCount = ai->adjustZombieCount(zombieCount);

	for (int i = 0; i < zombieCount; ++i) {
		Zombie* zombie = nullptr;
		int type = rand() % 100;

		// Increased special zombie spawning
		if (type < 40) {  // 40% Boomer
			zombie = new Boomer("boomer_" + std::to_string(i), "Boomer");
		}
		else if (type < 65) {  // 25% Spitter
			zombie = new Spitter("spitter_" + std::to_string(i), "Spitter");
		}
		else if (type < 85) {  // 20% Smoker
			zombie = new Smoker("smoker_" + std::to_string(i), "Smoker");
		}
		else {  // 15% Tank
			zombie = new Tank("tank_" + std::to_string(i), "Tank");
		}

		currentWave.enqueue(zombie);
	}

	std::cout << "" << zombieCount << " zombies appear!\n\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(800));
}

CombatResult GameplayEngine::conductCombat() {
	CombatResult result;
	Zombie* currentZombie = nullptr;

	while (!currentWave.isEmpty() || currentZombie != nullptr) {
		system(CLEAR_SCREEN);
		std::cout << "\n  COMBAT\n\n";



		if (currentZombie == nullptr || currentZombie->getHealth() <= 0) {
			if (currentZombie != nullptr) {
				std::cout << "  [KILL] " << currentZombie->getType() << " defeated!\n";

				combatActionHistory.push("Killed " + currentZombie->getType());
				
				// Trigger zombie death effects (e.g., Boomer explosion)
				currentZombie->onDeath(currentPlayer);
				
				// Award XP
				int xpGain = 10;
				currentPlayer->gainExperience(xpGain);
				std::cout << "  [+XP] Gained " << xpGain << " experience!\n\n";
				
				AudioEngine::getInstance()->playZombieDeathSound();
				result.zombiesKilled++;
				delete currentZombie;
				currentZombie = nullptr;
			}

			if (currentWave.isEmpty()) {
				if (currentWaveNumber < maxWavesPerLocation) {
					std::cout << "Next wave...\n\n";
					std::this_thread::sleep_for(std::chrono::milliseconds(1500));
					spawnZombieWave();
					continue;
				}
				else {
					break;
				}
			}

			currentZombie = currentWave.dequeue();
		}

		// Display zombie with health bar
		if (currentZombie) {
			int zombieHP = currentZombie->getHealth();
			int zombieMaxHP = currentZombie->getMaxHealth();
			float zombieHPPercent = (float)zombieHP / zombieMaxHP;
			int barWidth = 20;
			int filled = (int)(zombieHPPercent * barWidth);
			
			std::cout << "  Enemy: " << currentZombie->getType() << "\n";
			std::cout << "  [";
			for (int i = 0; i < barWidth; i++) {
				if (i < filled) std::cout << "=";
				else std::cout << " ";
			}
			std::cout << "] " << (int)(zombieHPPercent * 100) << "%\n";
			std::cout << "  HP: " << zombieHP << "/" << zombieMaxHP << "\n";
			std::cout << "  Remaining: " << currentWave.size() << "\n\n";
		}

		// Display player stats
		std::cout << "  YOU: " << currentPlayer->getHealth() << "/" << currentPlayer->getMaxHealth() << " HP\n";
		std::cout << "  Weapon: " << currentPlayer->getEquippedWeapon() << "\n";

		// Display combat history (latest actions first)
		if (!combatActionHistory.isEmpty()) {
			std::cout << "\n  RECENT ACTIONS:\n";

			// Create array to store latest actions
			std::string actions[MAX_COMBAT_HISTORY];
			int actionCount = 0;

			// Pop all actions into temp stack to reverse order
			Stack<std::string> tempStack;
			while (!combatActionHistory.isEmpty()) {
				tempStack.push(combatActionHistory.pop());
			}

			// Restore to original and collect latest MAX_COMBAT_HISTORY actions
			while (!tempStack.isEmpty()) {
				std::string action = tempStack.pop();
				combatActionHistory.push(action);  // Restore to original stack

				// Store in array (shift older actions down)
				if (actionCount < MAX_COMBAT_HISTORY) {
					actions[actionCount++] = action;
				} else {
					// Shift all actions down and add new one at end
					for (int i = 0; i < MAX_COMBAT_HISTORY - 1; i++) {
						actions[i] = actions[i + 1];
					}
					actions[MAX_COMBAT_HISTORY - 1] = action;
				}
			}

			// Display latest actions first (reverse array order)
			for (int i = actionCount - 1; i >= 0; i--) {
				std::cout << "  [" << (actionCount - i) << "] " << actions[i] << "\n";
			}
		}
		std::cout << "\n";

		std::cout << "  [1] Attack  [2] Dodge  [3] Item  [4] Flee\n";
		std::cout << "  Choice: ";

		int action;
		std::cin >> action;
		std::cin.ignore();

		switch (action) {
		case 1: { // Attack
			AudioEngine::getInstance()->playCombatAttackSound();
			int baseDamage = currentPlayer->getDamage() + (rand() % 5) - 2;
			
			// Apply hunger penalty
			int hunger = currentPlayer->getHunger();
			float hungerMultiplier = 1.0f;
			if (hunger <= 30) {
				hungerMultiplier = 0.5f; // 50% damage when hungry
				std::cout << "\n  [HUNGRY] Your attacks are weakened!\n";
			}
			
			int damage = (int)(baseDamage * hungerMultiplier);

			if ((rand() % 100) < 80) {
				std::cout << "\n  [HIT] Deal " << damage << " damage!\n";
				AudioEngine::getInstance()->playCombatHitSound();
				currentZombie->takeDamage(damage);
				result.playerDamageDealt += damage;

				combatActionHistory.push("Dealt " + std::to_string(damage) + " dmg to " + currentZombie->getType());

				if (currentZombie->getHealth() > 0) {
					// Zombie counter-attack with special ability chance
					int zombieAttackDmg = currentZombie->getDamage();
					
					// Check if zombie uses special ability
					if (currentZombie->canUseSpecialAbility() && rand() % 100 < currentZombie->getSpecialAbilityChance()) {
						std::string abilityMsg = currentZombie->useSpecialAbility(currentPlayer);
						std::cout << "  [SPECIAL] " << abilityMsg << "\n";
						
						// Apply special damage (usually higher)
						int specialDmg = (int)(zombieAttackDmg * 1.5f);
						std::cout << "  Zombie special attack for " << specialDmg << " damage!\n";
						currentPlayer->takeDamage(specialDmg);
						result.playerDamageTaken += specialDmg;

						combatActionHistory.push("Took " + std::to_string(specialDmg) + " dmg from " + currentZombie->getType());
					} else {
						std::cout << "  Zombie attacks for " << zombieAttackDmg << " damage!\n";
						currentPlayer->takeDamage(zombieAttackDmg);
						result.playerDamageTaken += zombieAttackDmg;
						combatActionHistory.push("Took " + std::to_string(zombieAttackDmg) + " dmg from " + currentZombie->getType());
					}
				}
			}
			else {
				std::cout << "\n  [MISS] Attack missed!\n";
				AudioEngine::getInstance()->playCombatMissSound();
				
				// Zombie still attacks on miss
				int zombieAttackDmg = currentZombie->getDamage();
				std::cout << "  Zombie punishes your miss for " << zombieAttackDmg << " damage!\n";
				currentPlayer->takeDamage(zombieAttackDmg);
				result.playerDamageTaken += zombieAttackDmg;
			}
			break;
		}

		case 2: { // Dodge
			if (rand() % 100 < 40) {
				std::cout << "\n  [DODGE] Avoided!\n";
				combatActionHistory.push("Dodged " + currentZombie->getType() + "'s attack");
			}
			else {
				int dmg = currentZombie->getDamage() / 2;
				std::cout << "\n  [PARTIAL] Take " << dmg << " damage!\n";
				currentPlayer->takeDamage(dmg);
				result.playerDamageTaken += dmg;
				combatActionHistory.push("Took " + std::to_string(dmg) + " dmg (partial dodge)");
			}
			break;
		}

		case 3: { // Use Item
			displayInventory();
			break;
		}

		case 4: { // Flee
			if (rand() % 100 < 50) {
				std::cout << "\n  [ESCAPED]\n";
				combatActionHistory.push("Fled from combat");
				result.playerWon = false;
				while (!currentWave.isEmpty()) delete currentWave.dequeue();
				if (currentZombie) delete currentZombie;
				std::cout << "\n  Press ENTER...";
				std::cin.get();
				inCombat = false;
				return result;
			}
			else {
				std::cout << "\n  [FAILED] Couldn't escape!\n";
				combatActionHistory.push("Failed to flee");
				// Zombie punishes failed escape
				int zombieAttackDmg = currentZombie->getDamage();
				std::cout << "  Zombie attacks while you flee for " << zombieAttackDmg << " damage!\n";
				currentPlayer->takeDamage(zombieAttackDmg);
				result.playerDamageTaken += zombieAttackDmg;
				combatActionHistory.push("Took " + std::to_string(zombieAttackDmg) + " dmg (flee failed)");
			}
			break;
		}
		}

		if (currentPlayer->getHealth() <= 0) {
			std::cout << "\n  [GAME OVER]\n";
			result.playerWon = false;
			inCombat = false;
			std::cout << "\n  Press ENTER...";
			std::cin.get();
			return result;
		}

		std::cout << "\n  Press ENTER...";
		std::cin.get();
	}

	result.playerWon = true;
	inCombat = false;

	system(CLEAR_SCREEN);
	std::cout << "\n  [VICTORY]\n\n";
	std::cout << "  Kills: " << result.zombiesKilled << "\n";
	std::cout << "  Damage Dealt: " << result.playerDamageDealt << "\n";
	std::cout << "  Damage Taken: " << result.playerDamageTaken << "\n";
	std::cout << "  Press ENTER...";
	std::cin.get();

	return result;
}

void GameplayEngine::handleCombatRound() {
	// Handled in conductCombat
}

void GameplayEngine::processZombieDeath(Zombie* zombie) {
	if (zombie) delete zombie;
}

void GameplayEngine::endCombat(const CombatResult& result) {
	// Display in conductCombat
}

// Placeholder combat helper methods
void GameplayEngine::playerAttack(Zombie* zombie, bool isRanged) {
	// Placeholder
}

int GameplayEngine::playerDodge() {
	return 0; // Placeholder
}

bool GameplayEngine::playerUseItem(const std::string& itemName) {
	return false; // Placeholder
}

Zombie* GameplayEngine::getNextZombie() {
	if (!currentWave.isEmpty()) {
		return currentWave.dequeue();
	}
	return nullptr;
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

Direction GameplayEngine::stringToDirection(const std::string& input) {
	if (input == "up" || input == "north") return Direction::UP;
	if (input == "down" || input == "south") return Direction::DOWN;
	if (input == "left" || input == "west") return Direction::LEFT;
	if (input == "right" || input == "east") return Direction::RIGHT;
	return Direction::CENTER;
}

std::string GameplayEngine::directionToString(Direction direction) {
	switch (direction) {
	case Direction::UP: return "UP";
	case Direction::DOWN: return "DOWN";
	case Direction::LEFT: return "LEFT";
	case Direction::RIGHT: return "RIGHT";
	default: return "CENTER";
	}
}

void GameplayEngine::displayPlayerStatus() {
	system(CLEAR_SCREEN);
	std::cout << "\n" << std::string(60, '=') << "\n";
	std::cout << "  PLAYER STATUS\n";
	std::cout << std::string(60, '=') << "\n\n";
	
	std::cout << "  Name: " << currentPlayer->getName() << "\n";
	std::cout << "  Level: " << currentPlayer->getLevel() << "\n\n";
	
	// Health bar
	int hp = currentPlayer->getHealth();
	int maxHP = currentPlayer->getMaxHealth();
	float hpPercent = (float)hp / maxHP;
	int barWidth = 30;
	int filled = (int)(hpPercent * barWidth);
	
	std::cout << "  Health: " << hp << "/" << maxHP << "\n";
	std::cout << "  [";
	for (int i = 0; i < barWidth; i++) {
		if (i < filled) std::cout << "=";
		else std::cout << " ";
	}
	std::cout << "]\n\n";
	
	// Hunger bar
	int hunger = currentPlayer->getHunger();
	int maxHunger = currentPlayer->getMaxHunger();
	float hungerPercent = (float)hunger / maxHunger;
	int hungerFilled = (int)(hungerPercent * barWidth);
	
	std::cout << "  Hunger: " << hunger << "/" << maxHunger << "\n";
	std::cout << "  [";
	for (int i = 0; i < barWidth; i++) {
		if (i < hungerFilled) std::cout << "=";
		else std::cout << " ";
	}
	std::cout << "]\n\n";
	
	// XP bar
	int xp = currentPlayer->getExperience();
	int xpToNext = currentPlayer->getExperienceToNextLevel();
	float xpPercent = (float)xp / xpToNext;
	int xpFilled = (int)(xpPercent * barWidth);
	
	std::cout << "  XP: " << xp << "/" << xpToNext << "\n";
	std::cout << "  [";
	for (int i = 0; i < barWidth; i++) {
		if (i < xpFilled) std::cout << "=";
		else std::cout << " ";
	}
	std::cout << "]\n";
	std::cout << "  Skill Points: " << currentPlayer->getSkillPoints() << "\n\n";
	
	// Combat stats
	std::cout << "  Equipped: " << currentPlayer->getEquippedWeapon() << "\n";
	std::cout << "  Damage: " << currentPlayer->getDamage() << "\n";
	std::cout << "  Inventory: " << currentPlayer->getInventorySize() << "/"
		<< currentPlayer->getMaxInventorySpace() << "\n\n";
	
	std::cout << std::string(60, '=') << "\n";
	std::cout << "  Press ENTER...";
	std::cin.get();
}

void GameplayEngine::displayInventory() {
	system(CLEAR_SCREEN);
	std::cout << "\n  INVENTORY\n\n";

	SinglyLinkedList<Item>& inv = currentPlayer->getInventory();
	if (inv.isEmpty()) {
		std::cout << "  Empty.\n";
		std::cout << "\n  Press ENTER...";
		std::cin.get();
		return;
	}

	// Display items
	int count = 1;
	for (auto it = inv.begin(); it != inv.end(); ++it) {
		std::cout << "  [" << count++ << "] " << (*it).getName()
			<< " (x" << (*it).getQuantity() << ")";
		
		// Show item type and stats
		if ((*it).getCategory() == Item::Category::WEAPON) {
			std::cout << " - Weapon (+";
			std::cout << (*it).getDamageBoost() << " DMG)";
			
			// Show ammo for ranged weapons
			if ((*it).getMaxAmmo() > 0) {
				std::cout << " [" << (*it).getAmmo() << "/" << (*it).getMaxAmmo() << " ammo]";
			}
			
			// Show durability
			int durability = (*it).getDurability();
			int maxDurability = (*it).getMaxDurability();
			if (maxDurability > 0) {
				float durPercent = (*it).getDurabilityPercent();
				std::cout << " Durability: " << durability << "/" << maxDurability;
				if (durPercent <= 0.25f) {
					std::cout << " [BREAKING!]";
				}
			}
		}
		else if ((*it).getCategory() == Item::Category::MEDICAL) {
			std::cout << " - Heals " << (*it).getHealthRestore() << " HP";
		}
		else if ((*it).getCategory() == Item::Category::FOOD) {
			std::cout << " - Food (+30 Hunger)";
		}
		std::cout << "\n";
	}

	std::cout << "\n  [U] Use Item  [E] Equip Weapon  [D] Drop Item  [0] Back\n";
	std::cout << "  Choice: ";
	
	char choice;
	std::cin >> choice;
	std::cin.ignore();

	if (choice == 'u' || choice == 'U') {
		std::cout << "\n  Enter item number to use: ";
		int itemNum;
		std::cin >> itemNum;
		std::cin.ignore();

		if (itemNum > 0 && itemNum <= currentPlayer->getInventorySize()) {
			// Get item at index
			int idx = 1;
			for (auto it = inv.begin(); it != inv.end(); ++it) {
				if (idx == itemNum) {
					const Item& item = *it;
					
					// Use item if consumable
					if (item.isConsumable() && item.isUsable()) {
						bool itemUsed = false;
						
						// Healing items
						if (item.getHealthRestore() > 0) {
							int newHP = currentPlayer->getHealth() + item.getHealthRestore();
							if (newHP > currentPlayer->getMaxHealth()) {
								newHP = currentPlayer->getMaxHealth();
							}
							currentPlayer->setHealth(newHP);
							std::cout << "\n  [USED] " << item.getName() << "! Restored "
								<< item.getHealthRestore() << " HP.\n";
							itemUsed = true;

							// Track in combat history if in combat
							if (inCombat) {
								combatActionHistory.push("Used " + item.getName() + " (+" + std::to_string(item.getHealthRestore()) + " HP)");
							}
						}

						// Food items (restore hunger)
						if (item.getCategory() == Item::Category::FOOD) {
							int hungerRestore = 30; // Default food restores 30 hunger
							int newHunger = currentPlayer->getHunger() + hungerRestore;
							if (newHunger > currentPlayer->getMaxHunger()) {
								newHunger = currentPlayer->getMaxHunger();
							}
							currentPlayer->setHunger(newHunger);
							std::cout << "\n  [USED] " << item.getName() << "! Restored "
								<< hungerRestore << " Hunger.\n";
							itemUsed = true;

							// Track in combat history if in combat
							if (inCombat) {
								combatActionHistory.push("Used " + item.getName() + " (+" + std::to_string(hungerRestore) + " Hunger)");
							}
						}
						
						if (itemUsed) {
							// Remove item (decrease quantity)
							currentPlayer->removeItem(item);
							std::cout << "  Press ENTER...";
							std::cin.get();
						}
					}
					else {
						std::cout << "\n  [ERROR] Cannot use this item.\n";
						std::cout << "  Press ENTER...";
						std::cin.get();
					}
					break;
				}
				idx++;
			}
		}
	}
	else if (choice == 'e' || choice == 'E') {
		std::cout << "\n  Enter weapon number to equip (0 to unequip): ";
		int weaponNum;
		std::cin >> weaponNum;
		std::cin.ignore();

		if (weaponNum == 0) {
			// Unequip weapon
			currentPlayer->setEquippedWeapon("Fists");
			currentPlayer->setDamage(8); // Reset to base damage
			std::cout << "\n  [UNEQUIPPED] Weapon removed.\n";
			std::cout << "  Damage: 8 (base)\n";
			std::cout << "  Press ENTER...";
			std::cin.get();
		}
		else if (weaponNum > 0 && weaponNum <= currentPlayer->getInventorySize()) {
			int idx = 1;
			for (auto it = inv.begin(); it != inv.end(); ++it) {
				if (idx == weaponNum) {
					const Item& item = *it;
					if (item.getCategory() == Item::Category::WEAPON) {
						// Apply weapon damage boost
						int baseDamage = 8; // Base fist damage
						int newDamage = baseDamage + item.getDamageBoost();
						currentPlayer->setDamage(newDamage);
						currentPlayer->setEquippedWeapon(item.getName());
						std::cout << "\n  [EQUIPPED] " << item.getName() << "!\n";
						std::cout << "  Damage: " << currentPlayer->getDamage() << " (+" << item.getDamageBoost() << " from weapon)\n";
						std::cout << "  Press ENTER...";
						std::cin.get();
					}
					else {
						std::cout << "\n  [ERROR] Not a weapon.\n";
						std::cout << "  Press ENTER...";
						std::cin.get();
					}
					break;
				}
				idx++;
			}
		}
	}
	else if (choice == 'd' || choice == 'D') {
		std::cout << "\n  Enter item number to drop: ";
		int dropNum;
		std::cin >> dropNum;
		std::cin.ignore();

		if (dropNum > 0 && dropNum <= currentPlayer->getInventorySize()) {
			int idx = 1;
			for (auto it = inv.begin(); it != inv.end(); ++it) {
				if (idx == dropNum) {
					const Item& item = *it;
					std::cout << "\n  [DROPPED] " << item.getName() << "\n";
					currentPlayer->removeItem(item);
					std::cout << "  Freed " << item.getInventorySpace() << " slots.\n";
					std::cout << "  Press ENTER...";
					std::cin.get();
					break;
				}
				idx++;
			}
		}
	}
}


void GameplayEngine::displayCollectedClues() {
	system(CLEAR_SCREEN);
	std::cout << "\n  CLUE JOURNAL\n\n";
	if (journal) {
		journal->displayProgress();
		if (journal->getTotalCollected() > 0) {
			std::cout << "\n  COLLECTED CLUES:\n\n";
			
			// Display clue list with numbers
			int count = 1;
			std::vector<Clue> cluesList;
			
			// Copy clues to vector for safe iteration
			DoublyLinkedList<Clue>& clues = journal->getClues();
			for (auto it = clues.begin(); it != clues.end(); ++it) {
				cluesList.push_back(*it);
			}
			
			// Display from vector
			for (const auto& clue : cluesList) {
				std::cout << "  [" << count++ << "] " << clue.getClueName() 
					<< " (" << clue.getLocationFound() << ")\n";
			}
			
			std::cout << "\n  [R] Read Clue  [0] Back\n";
			std::cout << "  Choice: ";
			
			char choice;
			std::cin >> choice;
			std::cin.ignore();
			
			if (choice == 'r' || choice == 'R') {
				std::cout << "\n  Enter clue number to read: ";
				int clueNum;
				std::cin >> clueNum;
				std::cin.ignore();
				
				if (clueNum > 0 && clueNum <= static_cast<int>(cluesList.size())) {
					const Clue& selectedClue = cluesList[clueNum - 1];
					system(CLEAR_SCREEN);
					std::cout << "\n" << std::string(80, '=') << "\n";
					std::cout << "  " << selectedClue.getClueName() << "\n";
					std::cout << std::string(80, '=') << "\n\n";
					std::cout << "  Location: " << selectedClue.getLocationFound() << "\n\n";
					std::cout << "  " << selectedClue.getContent() << "\n\n";
					std::cout << std::string(80, '=') << "\n";
					std::cout << "\n  Press ENTER...";
					std::cin.get();
				}
			}
		}
		else {
			std::cout << "\n  No clues collected yet.\n";
		}
	}
	std::cout << "\n  Press ENTER...";
	std::cin.get();
}

// ============================================================================
// LOCATION TRAVEL
// ============================================================================

bool GameplayEngine::canTravelToNewLocation() const {
	return hasExploredNewArea;
}

void GameplayEngine::displayTravelOptions() {
	system(CLEAR_SCREEN);
	std::cout << "\n  TRAVEL OPTIONS\n\n";
	std::cout << "  Current: " << currentLocation->getName() << "\n\n";
	std::cout << "  Available:\n\n";

	SinglyLinkedList<std::string>& connections = currentLocation->getConnections();

	if (connections.isEmpty()) {
		std::cout << "  No locations available.\n";
		std::cout << "\n  Press ENTER...";
		std::cin.get();
		return;
	}

	int i = 1;
	GameEngine* engine = GameEngine::getInstance();
	for (auto it = connections.begin(); it != connections.end(); ++it) {
		// Get the location object to display its name
		Location* loc = engine->getLocationByID(*it);
		if (loc) {
			std::cout << "  [" << i++ << "] " << loc->getName() << "\n";
		}
		else {
			std::cout << "  [" << i++ << "] " << *it << "\n";
		}
	}
	std::cout << "  [0] Cancel\n";
}

bool GameplayEngine::travelToLocation(const std::string& locationID) {
	GameEngine* engine = GameEngine::getInstance();
	Location* newLocation = engine->getLocationByID(locationID);

	if (!newLocation) {
		return false;
	}

	// CRITICAL: Update BOTH GameplayEngine and GameEngine locations for synchronization
	setCurrentLocation(newLocation);
	engine->setCurrentLocation(newLocation);  // Keep GameEngine in sync

	system(CLEAR_SCREEN);
	std::cout << "\n  Traveling to " << newLocation->getName() << "...\n\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	// Display chapter intro if first visit
	if (!newLocation->isVisited()) {
		newLocation->displayChapterIntro();
		newLocation->markVisited();
		std::cout << "\n\n  Press ENTER to continue...";
		std::cin.get();
	}

	return true;
}

