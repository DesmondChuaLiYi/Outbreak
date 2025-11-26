#include "SaveEngine.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdio>

// Initialize static members
const int SaveEngine::MAX_SAVE_FILES = 10;
const std::string SaveEngine::SAVE_FILE_PREFIX = "save_slot_";
const std::string SaveEngine::SAVE_FILE_EXTENSION = ".txt";
const std::string SaveEngine::INVENTORY_DELIMITER = "|";
const std::string SaveEngine::ITEM_DELIMITER = "~";

// Get save file path for a slot
std::string SaveEngine::getSaveFilePath(int slotNumber) {
	if (slotNumber < 1 || slotNumber > MAX_SAVE_FILES) {
		return "";
	}
	return SAVE_FILE_PREFIX + std::to_string(slotNumber) + SAVE_FILE_EXTENSION;
}

// Save player data to specific slot
bool SaveEngine::saveGameToSlot(const Player& player, int slotNumber) {
	if (slotNumber < 1 || slotNumber > MAX_SAVE_FILES) {
		std::cerr << "[ERROR] Invalid slot number. Must be between 1 and " << MAX_SAVE_FILES << ".\n";
		return false;
	}

	std::string filePath = getSaveFilePath(slotNumber);
	std::ofstream saveFile(filePath);

	if (!saveFile.is_open()) {
		std::cerr << "[ERROR] Could not open save file for writing!\n";
		return false;
	}

	try {
		// Write player basic info
		saveFile << player.getName() << INVENTORY_DELIMITER
			<< player.getLevel() << INVENTORY_DELIMITER
			<< player.getDamage() << INVENTORY_DELIMITER
			<< player.getHealth() << INVENTORY_DELIMITER
			<< player.getMaxHealth() << "\n";

		// Write inventory items
		SinglyLinkedList<Item>& inventory = const_cast<Player&>(player).getInventory();
		SinglyLinkedList<Item>::Iterator it = inventory.begin();
		while (it != inventory.end()) {
			saveFile << serializeItem(*it) << "\n";
			++it;
		}

		saveFile.close();
		std::cout << "[SUCCESS] Game saved to Slot " << slotNumber << "!\n";
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "[ERROR] Exception during save: " << e.what() << "\n";
		saveFile.close();
		return false;
	}
}

// Save player data to file (auto-finds next available slot)
bool SaveEngine::saveGame(const Player& player) {
	// Find the first available slot
	for (int i = 1; i <= MAX_SAVE_FILES; ++i) {
		if (!saveFileExists(i)) {
			return saveGameToSlot(player, i);
		}
	}
	
	// If all slots full, overwrite the last one
	return saveGameToSlot(player, MAX_SAVE_FILES);
}

// Load player data from specific slot
bool SaveEngine::loadGameFromSlot(Player& player, int slotNumber) {
	if (slotNumber < 1 || slotNumber > MAX_SAVE_FILES) {
		std::cerr << "[ERROR] Invalid slot number. Must be between 1 and " << MAX_SAVE_FILES << ".\n";
		return false;
	}

	std::string filePath = getSaveFilePath(slotNumber);
	std::ifstream saveFile(filePath);

	if (!saveFile.is_open()) {
		std::cerr << "[ERROR] Save file not found at slot " << slotNumber << "!\n";
		return false;
	}

	try {
		std::string line;

		// Read player basic info
		if (std::getline(saveFile, line)) {
			std::istringstream iss(line);
			std::string name, level, damage, health, maxHealth;

			std::getline(iss, name, '|');
			std::getline(iss, level, '|');
			std::getline(iss, damage, '|');
			std::getline(iss, health, '|');
			std::getline(iss, maxHealth, '|');

			// Create new player with loaded data
			int levelInt = std::stoi(level);
			int damageInt = std::stoi(damage);
			int healthInt = std::stoi(health);
			int maxHealthInt = std::stoi(maxHealth);

			// Reinitialize player
			player = Player("player_001", name, levelInt, damageInt, healthInt, maxHealthInt);

			// Read inventory items
			while (std::getline(saveFile, line)) {
				if (!line.empty()) {
					Item loadedItem = deserializeItem(line);
					player.addItem(loadedItem);
				}
			}
		}

		saveFile.close();
		std::cout << "[SUCCESS] Game loaded from Slot " << slotNumber << "!\n";
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "[ERROR] Exception during load: " << e.what() << "\n";
		saveFile.close();
		return false;
	}
}

// Load player data from file (with slot selection)
bool SaveEngine::loadGame(Player& player) {
	std::vector<SaveFileInfo> saveFiles = getAllSaveFiles();

	if (saveFiles.empty()) {
		std::cerr << "[ERROR] No save files found.\n";
		return false;
	}

	int slotNumber = displayProfileSelection();
	if (slotNumber == -1) {
		return false; // User cancelled
	}

	return loadGameFromSlot(player, slotNumber);
}

// Check if save file exists at specific slot
bool SaveEngine::saveFileExists(int slotNumber) {
	std::string filePath = getSaveFilePath(slotNumber);
	std::ifstream file(filePath);
	return file.good();
}

// Delete save file at specific slot
bool SaveEngine::deleteSaveFile(int slotNumber) {
	if (slotNumber < 1 || slotNumber > MAX_SAVE_FILES) {
		std::cerr << "[ERROR] Invalid slot number.\n";
		return false;
	}

	std::string filePath = getSaveFilePath(slotNumber);
	if (std::remove(filePath.c_str()) == 0) {
		std::cout << "[SUCCESS] Save file at Slot " << slotNumber << " deleted.\n";
		return true;
	}
	std::cerr << "[ERROR] Could not delete save file at Slot " << slotNumber << ".\n";
	return false;
}

// Get all available save files
std::vector<SaveEngine::SaveFileInfo> SaveEngine::getAllSaveFiles() {
	std::vector<SaveFileInfo> saveFiles;

	for (int i = 1; i <= MAX_SAVE_FILES; ++i) {
		SaveFileInfo info = getSaveFileInfo(i);
		if (info.isValid) {
			saveFiles.push_back(info);
		}
	}

	return saveFiles;
}

// Get player info from save file without loading full data
SaveEngine::SaveFileInfo SaveEngine::getSaveFileInfo(int slotNumber) {
	SaveFileInfo info;
	info.slotNumber = slotNumber;
	info.isValid = false;

	std::string filePath = getSaveFilePath(slotNumber);
	std::ifstream saveFile(filePath);

	if (!saveFile.is_open()) {
		info.filePath = filePath;
		return info;
	}

	try {
		std::string line;
		if (std::getline(saveFile, line)) {
			std::istringstream iss(line);
			std::string name, level;

			std::getline(iss, name, '|');
			std::getline(iss, level, '|');

			info.playerName = name;
			info.playerLevel = std::stoi(level);
			info.filePath = filePath;
			info.isValid = true;
		}
		saveFile.close();
	}
	catch (const std::exception& e) {
		std::cerr << "[ERROR] Could not read save file info: " << e.what() << "\n";
	}

	return info;
}

// Display all profiles for selection
int SaveEngine::displayProfileSelection() {
	std::vector<SaveFileInfo> saveFiles = getAllSaveFiles();

	if (saveFiles.empty()) {
		std::cout << "\n[INFO] No save files found.\n";
		return -1;
	}

	std::cout << "\n========================================\n";
	std::cout << "   LOAD GAME - SELECT PROFILE\n";
	std::cout << "========================================\n";

	for (size_t i = 0; i < saveFiles.size(); ++i) {
		std::cout << "[" << (i + 1) << "] Slot " << saveFiles[i].slotNumber 
			<< " - " << saveFiles[i].playerName 
			<< " (Level " << saveFiles[i].playerLevel << ")\n";
	}

	std::cout << "[0] Cancel\n";
	std::cout << "========================================\n";
	std::cout << "Enter your choice: ";

	int choice;
	std::cin >> choice;
	std::cin.ignore();

	if (choice == 0) {
		return -1;
	}

	if (choice >= 1 && choice <= static_cast<int>(saveFiles.size())) {
		return saveFiles[choice - 1].slotNumber;
	}

	std::cout << "[ERROR] Invalid choice.\n";
	return -1;
}

// Serialize item to string format
std::string SaveEngine::serializeItem(const Item& item) {
	std::ostringstream oss;
	oss << item.getName() << ITEM_DELIMITER
		<< static_cast<int>(item.getCategory()) << ITEM_DELIMITER
		<< item.getDescription() << ITEM_DELIMITER
		<< item.getQuantity() << ITEM_DELIMITER
		<< item.getInventorySpace() << ITEM_DELIMITER
		<< item.isConsumable() << ITEM_DELIMITER
		<< item.isUsable() << ITEM_DELIMITER
		<< item.getHealthRestore() << ITEM_DELIMITER
		<< item.getHungerRestore() << ITEM_DELIMITER
		<< item.getInfectionCure() << ITEM_DELIMITER
		<< item.getDamageBoost() << ITEM_DELIMITER
		<< static_cast<int>(item.getEffectType()) << ITEM_DELIMITER
		<< item.getEffectTurns() << ITEM_DELIMITER
		<< item.getEffectPower();

	return oss.str();
}

// Deserialize item from string format
Item SaveEngine::deserializeItem(const std::string& itemData) {
	std::istringstream iss(itemData);
	std::string token;
	std::vector<std::string> tokens;

	// Split the item data by delimiter
	while (std::getline(iss, token, '~')) {
		tokens.push_back(token);
	}

	if (tokens.size() < 13) {
		throw std::runtime_error("Invalid item data format");
	}

	std::string name = tokens[0];
	Item::Category category = static_cast<Item::Category>(std::stoi(tokens[1]));
	std::string description = tokens[2];
	int quantity = std::stoi(tokens[3]);
	int invSpace = std::stoi(tokens[4]);
	bool consumable = std::stoi(tokens[5]) != 0;
	bool usable = std::stoi(tokens[6]) != 0;
	int healthRestore = std::stoi(tokens[7]);
	int hungerRestore = std::stoi(tokens[8]);
	int infectionCure = std::stoi(tokens[9]);
	int damageBoost = std::stoi(tokens[10]);
	Item::EffectType effectType = static_cast<Item::EffectType>(std::stoi(tokens[11]));
	int effectTurns = std::stoi(tokens[12]);
	int effectPower = (tokens.size() > 13) ? std::stoi(tokens[13]) : 0;

	Item loadedItem("item_loaded", name, category, description, quantity, invSpace, consumable, usable,
		healthRestore, hungerRestore, infectionCure, damageBoost, effectType, effectTurns, effectPower);

	return loadedItem;
}
