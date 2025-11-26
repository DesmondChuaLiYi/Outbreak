#ifndef CRAFTING_H
#define CRAFTING_H

#include "Item.h"
#include "SinglyLinkedList.h"
#include <string>
#include <vector>

// Recipe for crafting
struct CraftingRecipe {
	std::string recipeID;
	std::string recipeName;
	std::string description;
	Item resultItem;
	std::vector<std::pair<std::string, int>> materials; // {material_id, quantity_needed}
	int craftTime; // seconds

	CraftingRecipe(const std::string& id, const std::string& name, const std::string& desc,
		const Item& result, int time = 5)
		: recipeID(id), recipeName(name), description(desc), resultItem(result), craftTime(time) {}

	void addMaterial(const std::string& materialID, int quantity) {
		materials.push_back({materialID, quantity});
	}
};

// Crafting system manager
class CraftingSystem {
private:
	SinglyLinkedList<CraftingRecipe*> fRecipes;

public:
	CraftingSystem();
	~CraftingSystem();

	// Initialize recipes
	void initializeRecipes();

	// Recipe management
	void addRecipe(CraftingRecipe* recipe);
	CraftingRecipe* getRecipe(const std::string& recipeID);

	// Get all recipes
	SinglyLinkedList<CraftingRecipe*>& getRecipes();

	// Display available recipes
	void displayAvailableRecipes();

	// Check if player can craft
	bool canCraft(const CraftingRecipe* recipe, class Player* player);

	// Craft item
	bool craftItem(const CraftingRecipe* recipe, class Player* player);
};

#endif /* CRAFTING_H */
