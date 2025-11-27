#ifndef SKILLTREE_H
#define SKILLTREE_H
#include "SkillNode.h"
#include "HashTable.h"
#include "SinglyLinkedList.h"
#include <string>
#include <vector>

class SkillTree {
private:
	SinglyLinkedList<SkillNode*> fRootSkills; // Top-level skills
	HashTable<std::string, SkillNode*> fSkillLookup; // Map skill names to nodes for quick lookup
	int fAvailablePoints; // Points available to spend

	// Helper for tree traversal
	void displaySkillBranch(SkillNode* aNode, int aDepth) const;
	void calculateTotalBonusesRecursive(SkillNode* aNode, int& totalDamage, int& totalHealth,
		int& totalStamina, int& totalInfection, int& totalCrafting,
		float& totalCraftingFloat, float& totalScavenge) const;
	void countUnlockedRecursive(SkillNode* aNode, int& count) const;

public:
	// Constructor
	SkillTree();

	// Parameterised constructor
	SkillTree(int initialPoints);

	// Destructor
	virtual ~SkillTree();

	// Initialise the skill tree
	void initialiseDefaultTree();

	// Skill management
	bool unlockSkill(const std::string& skillName);
	bool levelUpSkill(const std::string& skillName);
	SkillNode* getSkill(const std::string& skillName) const;

	// Skill point management
	void addSkillPoints(int aPoints);
	void spendSkillPoints(int aPoints);
	int getAvailablePoints() const;

	// Tree operation
	void addRootSkill(SkillNode* aSkill);
	SinglyLinkedList<SkillNode*>& getRootSkills();

	// Calculate total bonuses from unlocked skills
	int getTotalDamageBonus() const;
	int getTotalHealthBonus() const;
	int getTotalStaminaBonus() const;
	int getTotalInfectionResistance() const;
	float getTotalCraftingBonus() const;
	float getTotalScavengeBonus() const;

	// Utility
	int getTotalUnlockedSkills() const;
	bool hasUnlockedSkill(const std::string& skillName) const;

	// Save/Load support
	void getUnlockedSkillData(std::vector<std::string>& skillIDs, std::vector<int>& levels) const;
	void restoreUnlockedSkills(const std::vector<std::string>& skillIDs, const std::vector<int>& levels);
};

#endif /* SKILLTREE_H */