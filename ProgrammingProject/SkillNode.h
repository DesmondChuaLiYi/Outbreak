#ifndef SKILLNODE_H
#define SKILLNODE_H
#include <iostream>
#include <string>
#include "SinglyLinkedList.h"

class SkillNode {
public:
	enum class SkillType {
		COMBAT, // Combat-related skills
		SURVIVAL, // Survival skills
		CRAFTING, // Crafting skills
		MEDICAL, // Better healing, infection resistance
		SCAVENGING // Find better loot
	};

private:
	std::string fSkillName;
	std::string fSkillDescription;
	SkillType fType;

	int fCost; // Skill points required to unlock
	int fLevel; // Skill level (0 = locked, 1 = unlocked, 1-3 = tiers)
	int fMaxLevel; // Maximum level for this skill
	bool fUnlocked; // Is the skill unlocked?

	// Store the skill ID for lookup
	std::string fSkillID;

	// Stat bonuses provided by this skill
	int fDamageBonus;
	int fHealthBonus;
	int fStaminaBonus;
	int fInfectionResistance; // Reduces infection chance
	float fCraftingSpeedBonus; // Reduces crafting material cost
	float fScavengeBonus; // Increases loot quantity/quality

	// Tree structure pointers
	SkillNode* fParent;
	SinglyLinkedList<SkillNode*> fChildren; // Skills to unlock

public:
	// Constructor
	SkillNode();

	// Parameterised constructor
	SkillNode(const std::string& aID, const std::string& aName, const std::string& aDescription, SkillType aType, int aCost, int aMaxLevel = 3);

	// Destructor
	virtual ~SkillNode();

	// Tree operations
	void addChild(SkillNode* aChild);
	void setParent(SkillNode* aParent);
	SkillNode* getParent() const;
	SinglyLinkedList<SkillNode*>& getChildren();
	const SinglyLinkedList<SkillNode*>& getChildren() const;

	// Getter methods
	std::string getSkillID() const;
	std::string getSkillName() const;
	std::string getSkillDescription() const;
	SkillType getSkillType() const;
	int getCost() const;
	int getLevel() const;
	int getMaxLevel() const;
	bool isUnlocked() const;

	// Setter methods
	void setDamageBonus(int aBonus);
	void setHealthBonus(int aBonus);
	void setStaminaBonus(int aBonus);
	void setInfectionResistance(int aResistance);
	void setCraftingSpeedBonus(float aBonus);
	void setScavengeBonus(float aBonus);

	// Stat bonus getters
	int getDamageBonus() const;
	int getHealthBonus() const;
	int getStaminaBonus() const;
	int getInfectionResistance() const;
	float getCraftingSpeedBonus() const;
	float getScavengeBonus() const;

	// Skill operations
	void unlockSkill();
	void levelUpSkill();
	bool canUnlock() const;
	bool isMaxLevel() const;

	// Utility
	std::string typeToString() const;
	void displaySkillInfo() const;
};

#endif /* SKILLNODE_H */