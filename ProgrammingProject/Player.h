#ifndef PLAYER_H
#define PLAYER_H
#include "Entity.h"
#include "SinglyLinkedList.h"
#include "Item.h"
#include "SkillTree.h"

class Player : public Entity {
private:
	int fLevel;
	int fDamage;
	int fHealth;
	int fMaxHealth;
	int fHunger;
	int fMaxHunger;
	int fExperience;
	int fExperienceToNextLevel;
	int fSkillPoints;
	int fMaxInventorySpace;
	int fCurrentInventorySpace;
	SinglyLinkedList<Item> fInventory;
	std::string fEquippedWeapon;
	SkillTree fSkillTree;

	// Base stats (without bonuses)
	int fBaseDamage;
	int fBaseMaxHealth;

public:
	// Constructor
	Player();

	// Parameterised constructor
	Player(const std::string& aID, const std::string aName, int aLevel, int aDamage, int aHealth, int aMaxHealth);

	// Inventory management methods
	void addItem(const Item& aItem);
	void removeItem(const Item& aItem);
	SinglyLinkedList<Item>& getInventory();
	int getInventorySize() const;
	int getMaxInventorySpace() const;
	void setMaxInventorySpace(int newMax);

	// Getter methods for player stats
	int getLevel() const;
	int getDamage() const;
	int getHealth() const;
	int getMaxHealth() const;
	int getHunger() const;
	int getMaxHunger() const;
	int getExperience() const;
	int getExperienceToNextLevel() const;
	int getSkillPoints() const;
	int getSpeed() const;
	std::string getEquippedWeapon() const;

	// NEW: Get base stats (without bonuses)
	int getBaseDamage() const;
	int getBaseMaxHealth() const;

	// NEW: Apply skill bonuses to current stats
	void applySkillBonuses();

	// NEW: Get loot quality modifier from scavenging
	float getLootQualityModifier() const;

	// NEW: Get crafting cost reduction modifier
	float getCraftingCostModifier() const;

	// Setter methods
	void setHealth(int aHealth);
	void setDamage(int aDamage);
	void setHunger(int aHunger);
	void setEquippedWeapon(const std::string& weaponName);
	void setXP(int xp) { fExperience = xp; }
	void setSkillPoints(int points) {
		fSkillPoints = points;
		// Sync with skill tree by calculating the difference
		int currentTreePoints = fSkillTree.getAvailablePoints();
		int difference = points - currentTreePoints;
		if (difference > 0) {
			fSkillTree.addSkillPoints(difference);
		} else if (difference < 0) {
			fSkillTree.spendSkillPoints(-difference);
		}
	}

	// Progression methods
	void gainExperience(int amount);
	void levelUp();
	void addSkillPoints(int points);
	void spendSkillPoint();

	// Skill tree access
	SkillTree& getSkillTree();

	// Combat method
	void takeDamage(int aDamage);

	// Override displayInformation method
	void displayInformation() override;
};

#endif /* PLAYER_H */