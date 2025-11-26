#include "Crafting.h"
#include "Player.h"
#include <iostream>

CraftingSystem::CraftingSystem() {
}

CraftingSystem::~CraftingSystem() {
	// Clean up recipes
	for (auto it = fRecipes.begin(); it != fRecipes.end(); ++it) {
		delete *it;
	}
}

void CraftingSystem::initializeRecipes() {
	// Recipe 1: First Aid Kit (from bandages and cloth)
	CraftingRecipe* firstAidRecipe = new CraftingRecipe(
		"craft_001",
		"First Aid Kit",
		"Combine bandages with cloth to create a more effective medical kit. +25 HP",
		Item("item_first_aid", "First Aid Kit", Item::Category::MEDICAL,
			"Restores 25 HP", 1, 4, true, true, 25, 0, 0, 0),
		3
	);
	firstAidRecipe->addMaterial("mat_bandages_woods", 1);  // 1 Bandage
	firstAidRecipe->addMaterial("mat_cloth", 1);      // 1 Cloth
	addRecipe(firstAidRecipe);

	// Recipe 2: Fortified Food (from food and herbs)
	CraftingRecipe* fortifiedFoodRecipe = new CraftingRecipe(
		"craft_002",
		"Fortified Food",
		"Mix food with herbs for better nutrition. +50 Hunger",
		Item("item_fortified_food", "Fortified Food", Item::Category::FOOD,
			"Restores 50 hunger", 1, 5, true, true, 0, 50, 0, 0),
		4
	);
	fortifiedFoodRecipe->addMaterial("mat_food_woods", 1); // 1 Food Rations
	fortifiedFoodRecipe->addMaterial("mat_herbs", 1);  // 1 Herbs
	addRecipe(fortifiedFoodRecipe);

	// Recipe 3: Healing Potion (from herbs + water)
	CraftingRecipe* healingPotionRecipe = new CraftingRecipe(
		"craft_003",
		"Healing Potion",
		"Brew a potion with herbs and water. +30 HP + 20 Hunger",
		Item("item_healing_potion", "Healing Potion", Item::Category::MEDICAL,
			"Restores 30 HP and 20 hunger", 1, 4, true, true, 30, 20, 0, 0),
		5
	);
	healingPotionRecipe->addMaterial("mat_herbs", 2);  // 2 Herbs
	healingPotionRecipe->addMaterial("mat_water", 1);  // 1 Water
	addRecipe(healingPotionRecipe);

	// Recipe 4: Infection Treatment (from antibiotics + bandages)
	CraftingRecipe* infectionTreatmentRecipe = new CraftingRecipe(
		"craft_004",
		"Infection Treatment",
		"Create a treatment for infection. Cures 50% infection",
		Item("item_infection_treatment", "Infection Treatment", Item::Category::MEDICAL,
			"Cures 50% infection", 1, 3, true, true, 0, 0, 50, 0),
		4
	);
	infectionTreatmentRecipe->addMaterial("mat_antibiotics", 2); // 2 Antibiotics
	infectionTreatmentRecipe->addMaterial("mat_bandages_cem", 1);   // 1 Bandage
	addRecipe(infectionTreatmentRecipe);

	// Recipe 5: Reinforced Weapon (from scrap metal + wire)
	CraftingRecipe* reinforcedWeaponRecipe = new CraftingRecipe(
		"craft_005",
		"Reinforced Weapon",
		"Add scrap metal to strengthen your weapon. +5 damage",
		Item("item_reinforced_weapon", "Reinforced Weapon", Item::Category::WEAPON,
			"Enhanced melee weapon. +5 bonus damage", 1, 8, false, true, 0, 0, 0, 5),
		6
	);
	reinforcedWeaponRecipe->addMaterial("mat_scrap", 3);    // 3 Scrap Metal
	addRecipe(reinforcedWeaponRecipe);
}

void CraftingSystem::addRecipe(CraftingRecipe* recipe) {
	if (recipe != nullptr) {
		fRecipes.pushBack(recipe);
	}
}

CraftingRecipe* CraftingSystem::getRecipe(const std::string& recipeID) {
	for (auto it = fRecipes.begin(); it != fRecipes.end(); ++it) {
		if ((*it)->recipeID == recipeID) {
			return *it;
		}
	}
	return nullptr;
}

SinglyLinkedList<CraftingRecipe*>& CraftingSystem::getRecipes() {
	return fRecipes;
}

void CraftingSystem::displayAvailableRecipes() {
	std::cout << "\n  AVAILABLE RECIPES:\n\n";

	int count = 1;
	for (auto it = fRecipes.begin(); it != fRecipes.end(); ++it) {
		CraftingRecipe* recipe = *it;
		std::cout << "  [" << count++ << "] " << recipe->recipeName << "\n";
		std::cout << "      " << recipe->description << "\n";
		std::cout << "      Materials needed:\n";

		for (const auto& mat : recipe->materials) {
			std::cout << "   - " << mat.first << " (x" << mat.second << ")\n";
		}
		std::cout << "\n";
	}
}

bool CraftingSystem::canCraft(const CraftingRecipe* recipe, Player* player) {
	if (recipe == nullptr || player == nullptr) {
		return false;
	}

	// Check if player has all required materials
	SinglyLinkedList<Item>& inventory = player->getInventory();

	for (const auto& materialReq : recipe->materials) {
		int neededCount = materialReq.second;
		int foundCount = 0;

		for (auto it = inventory.begin(); it != inventory.end(); ++it) {
			// Check both by ID and by Name for flexibility
			if ((*it).getID() == materialReq.first || (*it).getName() == materialReq.first) {
				foundCount += (*it).getQuantity();
			}
		}

		if (foundCount < neededCount) {
			return false; // Not enough of this material
		}
	}

	// Check if player has space for result
	if (player->getInventorySize() + recipe->resultItem.getInventorySpace() >
		player->getMaxInventorySpace()) {
		return false; // Not enough space
	}

	return true;
}

bool CraftingSystem::craftItem(const CraftingRecipe* recipe, Player* player) {
	if (!canCraft(recipe, player)) {
		return false;
	}

	// Remove materials from inventory
	SinglyLinkedList<Item>& inventory = player->getInventory();

	for (const auto& materialReq : recipe->materials) {
		int neededCount = materialReq.second;
		int removed = 0;

		// Iterate through inventory and remove matching materials
		auto it = inventory.begin();
		while (it != inventory.end()) {
			if ((*it).getID() == materialReq.first || (*it).getName() == materialReq.first) {
				int canRemove = std::min(neededCount - removed, (*it).getQuantity());
				
				// Create a copy to remove
				Item itemToRemove = *it;
				itemToRemove.setQuantity(canRemove);
				
				// Remove this amount
				player->removeItem(*it);
				removed += canRemove;

				if (removed >= neededCount) {
					break;
				}
				
				// Reset iterator after removal
				it = inventory.begin();
				continue;
			}
			++it;
		}
	}

	// Add crafted item to inventory
	player->addItem(recipe->resultItem);

	return true;
}
