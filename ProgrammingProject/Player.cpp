#include "Player.h"

// Constructor
Player::Player() : Entity(), fLevel(1), fDamage(15), fHealth(100), fMaxHealth(100),
fHunger(100), fMaxHunger(100), fExperience(0), fExperienceToNextLevel(100), fSkillPoints(0),
fMaxInventorySpace(20), fCurrentInventorySpace(0), fEquippedWeapon("Knife"), fSkillTree(0),
fBaseDamage(15), fBaseMaxHealth(100) {
	fSkillTree.initialiseDefaultTree();
}

// Parameterised constructor
Player::Player(const std::string& aID, const std::string aName, int aLevel, int aDamage, int aHealth, int aMaxHealth)
	: Entity(aName), fLevel(aLevel), fDamage(aDamage), fHealth(aHealth), fMaxHealth(aMaxHealth),
	fHunger(100), fMaxHunger(100), fExperience(0), fExperienceToNextLevel(100 * aLevel), fSkillPoints(0),
	fMaxInventorySpace(20), fCurrentInventorySpace(0), fEquippedWeapon("Knife"), fSkillTree(0),
	fBaseDamage(aDamage), fBaseMaxHealth(aMaxHealth) {
	fSkillTree.initialiseDefaultTree();
}

// Add item to inventory
void Player::addItem(const Item& aItem) {
	if (fCurrentInventorySpace + aItem.getInventorySpace() <= fMaxInventorySpace) {
		fInventory.pushBack(aItem);
		fCurrentInventorySpace += aItem.getInventorySpace();
	}
}

// Remove item from inventory
void Player::removeItem(const Item& aItem) {
	fInventory.remove(aItem);
	fCurrentInventorySpace -= aItem.getInventorySpace();
	if (fCurrentInventorySpace < 0) {
		fCurrentInventorySpace = 0;
	}
}

// Get reference to inventory
SinglyLinkedList<Item>& Player::getInventory() {
	return fInventory;
}

// Get current inventory space used
int Player::getInventorySize() const {
	return fCurrentInventorySpace;
}

// Get max inventory space
int Player::getMaxInventorySpace() const {
	return fMaxInventorySpace;
}

// Set max inventory space (for backpack upgrades)
void Player::setMaxInventorySpace(int newMax) {
	fMaxInventorySpace = newMax;
}

// Getter methods for player stats
int Player::getLevel() const {
	return fLevel;
}

int Player::getDamage() const {
	return fDamage;
}

int Player::getHealth() const {
	return fHealth;
}

int Player::getMaxHealth() const {
	return fMaxHealth;
}

int Player::getSpeed() const {
	return fLevel * 2;
}

int Player::getHunger() const {
	return fHunger;
}

int Player::getMaxHunger() const {
	return fMaxHunger;
}

std::string Player::getEquippedWeapon() const {
	return fEquippedWeapon;
}

// Getter for experience
int Player::getExperience() const {
	return fExperience;
}

// Getter for experience to next level
int Player::getExperienceToNextLevel() const {
	return fExperienceToNextLevel;
}

// Getter for skill points
int Player::getSkillPoints() const {
	return fSkillPoints;
}

// Setter for health
void Player::setHealth(int aHealth) {
	fHealth = aHealth;
	if (fHealth > fMaxHealth) {
		fHealth = fMaxHealth;
	}
	if (fHealth < 0) {
		fHealth = 0;
	}
}

// Setter for damage
void Player::setDamage(int aDamage) {
	fDamage = aDamage;
	if (fDamage < 0) {
		fDamage = 0;
	}
}

// Setter for hunger
void Player::setHunger(int aHunger) {
	fHunger = aHunger;
	if (fHunger > fMaxHunger) {
		fHunger = fMaxHunger;
	}
	if (fHunger < 0) {
		fHunger = 0;
	}
}

// Setter for equipped weapon
void Player::setEquippedWeapon(const std::string& weaponName) {
	fEquippedWeapon = weaponName;
}

// Gain experience and check for level up
void Player::gainExperience(int amount) {
	fExperience += amount;
	
	// Check for level up
	while (fExperience >= fExperienceToNextLevel) {
		levelUp();
	}
}

// Level up the player
void Player::levelUp() {
	fLevel++;
	fExperience -= fExperienceToNextLevel;
	
	// Calculate new XP requirement: 100 * level * 1.5
	fExperienceToNextLevel = (int)(100 * fLevel * 1.5);
	
	// Grant skill points
	fSkillPoints += 3;
	
	// Increase base stats
	fBaseDamage += 2;
	fBaseMaxHealth += 10;
	
	// Apply skill bonuses to updated base stats
	applySkillBonuses();
	
	// Full heal on level up
	fHealth = fMaxHealth;
	
	// Level up notification will be shown by GameplayEngine
}

// Add skill points
void Player::addSkillPoints(int points) {
	fSkillPoints += points;
	fSkillTree.addSkillPoints(points); // Sync with skill tree
}

// Spend a skill point
void Player::spendSkillPoint() {
	if (fSkillPoints > 0) {
		fSkillPoints--;
		fSkillTree.spendSkillPoints(1); // Sync with skill tree
	}
}

// Get skill tree reference
SkillTree& Player::getSkillTree() {
	return fSkillTree;
}

// Take damage and reduce health
void Player::takeDamage(int aDamage) {
	fHealth -= aDamage;
	if (fHealth < 0) {
		fHealth = 0;
	}
}

// Display player information
void Player::displayInformation() {
	std::cout << "Player: " << fName << "\n";
	std::cout << "Level: " << fLevel << "\n";
	std::cout << "Health: " << fHealth << "/" << fMaxHealth << "\n";
	std::cout << "Damage: " << fDamage << "\n";
	std::cout << "Inventory: " << fCurrentInventorySpace << "/" << fMaxInventorySpace << " slots\n";
}

// Get base damage (without bonuses)
int Player::getBaseDamage() const {
	return fBaseDamage;
}

// Get base max health (without bonuses)
int Player::getBaseMaxHealth() const {
	return fBaseMaxHealth;
}

// Apply skill bonuses to current stats
void Player::applySkillBonuses() {
	// Get bonuses from skill tree
	int damageBonus = fSkillTree.getTotalDamageBonus();
	int healthBonus = fSkillTree.getTotalHealthBonus();

	// Apply damage bonus
	fDamage = fBaseDamage + damageBonus;
	if (fDamage < 1) fDamage = 1;

	// Apply health bonus
	int newMaxHealth = fBaseMaxHealth + healthBonus;
	if (newMaxHealth < 1) newMaxHealth = 1;

	// If max health increased, scale current health proportionally
	if (newMaxHealth > fMaxHealth) {
		float healthPercentage = (float)fHealth / fMaxHealth;
		fHealth = (int)(healthPercentage * newMaxHealth);
	}

	fMaxHealth = newMaxHealth;

	// Clamp current health to max
	if (fHealth > fMaxHealth) {
		fHealth = fMaxHealth;
	}
}

// Get loot quality modifier from scavenging skills
float Player::getLootQualityModifier() const {
	float scavengeBonus = fSkillTree.getTotalScavengeBonus();
	// Base 1.0 means normal loot, 1.25 means 25% better loot
	return 1.0f + scavengeBonus;
}

// Get crafting cost reduction modifier
float Player::getCraftingCostModifier() const {
	float craftingBonus = fSkillTree.getTotalCraftingBonus();
	// Base 1.0 means normal cost, 0.75 means 25% cheaper
	return 1.0f - craftingBonus;
}