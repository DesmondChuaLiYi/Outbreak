#ifndef GAMESAVE_H
#define GAMESAVE_H
#include <fstream>
#include <string>
#include <vector>
#include "Player.h"
#include "Item.h"

class GameSave {
public:
	// Save file info structure
	struct SaveFileInfo {
		int slotNumber;
		std::string playerName;
		int playerLevel;
		std::string filePath;
		bool isValid;
	};

private:
	static const int MAX_SAVE_FILES;
	static const std::string SAVE_FILE_PREFIX;
	static const std::string SAVE_FILE_EXTENSION;
	static const std::string INVENTORY_DELIMITER;
	static const std::string ITEM_DELIMITER;

public:
	// Save player data to file (auto-finds next available slot)
	static bool saveGame(const Player& player);

	// Save player data to specific slot
	static bool saveGameToSlot(const Player& player, int slotNumber);

	// Load player data from file (with slot selection)
	static bool loadGame(Player& player);

	// Load player data from specific slot
	static bool loadGameFromSlot(Player& player, int slotNumber);

	// Get all available save files
	static std::vector<SaveFileInfo> getAllSaveFiles();

	// Check if save file exists at specific slot
	static bool saveFileExists(int slotNumber);

	// Delete save file at specific slot
	static bool deleteSaveFile(int slotNumber);

	// Display all profiles for selection
	static int displayProfileSelection();

private:
	// Helper methods for serialization
	static std::string serializeItem(const Item& item);
	static Item deserializeItem(const std::string& itemData);
	
	// Get save file path for a slot
	static std::string getSaveFilePath(int slotNumber);

	// Get player info from save file without loading full data
	static SaveFileInfo getSaveFileInfo(int slotNumber);
};

#endif /* GAMESAVE_H */
