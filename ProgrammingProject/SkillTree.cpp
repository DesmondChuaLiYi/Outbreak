#include "SkillTree.h"

// Constructor
SkillTree::SkillTree() : fAvailablePoints(0) {}

// Parameterised constructor
SkillTree::SkillTree(int initialPoints) : fAvailablePoints(initialPoints) {}

// Destructor
SkillTree::~SkillTree() {
	// Clean up skill nodes
	for (SkillNode* rootSkill : fRootSkills) {
		delete rootSkill;
	}
}

// Initialise the skill tree
void SkillTree::initialiseDefaultTree() {
	// Combat skill tree
	SkillNode* meleeBasics = new SkillNode("combat_melee_1", "Melee Combat", "Increases melee damage. +5 melee damage", SkillNode::SkillType::COMBAT, 1);
	meleeBasics->setDamageBonus(5);
	addRootSkill(meleeBasics);
	fSkillLookup.insert("combat_melee_1", meleeBasics);

	SkillNode* heavyHitter = new SkillNode("combat_melee_2", "Heavy Hitter", "Devastating strikes. +10 melee damage", SkillNode::SkillType::COMBAT, 2);
	heavyHitter->setDamageBonus(10);
	meleeBasics->addChild(heavyHitter);
	fSkillLookup.insert("combat_melee_2", heavyHitter);

	// Survival skill tree
	SkillNode* resilience = new SkillNode("survival_health_1", "Resilience", "Tougher constitution. +20 max health", SkillNode::SkillType::SURVIVAL, 1);
	resilience->setHealthBonus(20);
	addRootSkill(resilience);
	fSkillLookup.insert("survival_health_1", resilience);

	SkillNode* ironBody = new SkillNode("survival_health_2", "Iron Body", "Infection resistance. +30% infection resistance", SkillNode::SkillType::SURVIVAL, 2);
	ironBody->setInfectionResistance(30);
	resilience->addChild(ironBody);
	fSkillLookup.insert("survival_health_2", ironBody);

	// Medical skill tree
	SkillNode* firstAid = new SkillNode("medical_heal_1", "First Aid", "Better healing. +50% medkit effectiveness", SkillNode::SkillType::MEDICAL, 1);
	addRootSkill(firstAid);
	fSkillLookup.insert("medical_heal_1", firstAid);

	// Scavenging skill tree
	SkillNode* scavenger = new SkillNode("scavenge_loot_1", "Scavenger", "Find more items. +25% loot quality", SkillNode::SkillType::SCAVENGING, 1);
	scavenger->setScavengeBonus(0.25f);
	addRootSkill(scavenger);
	fSkillLookup.insert("scavenge_loot_1", scavenger);

	// Crafting skill tree
	SkillNode* resourceful = new SkillNode("craft_efficiency_1", "Resourceful", "Efficient crafting. -25% material cost", SkillNode::SkillType::CRAFTING, 1);
	resourceful->setCraftingSpeedBonus(0.25f);
	addRootSkill(resourceful);
	fSkillLookup.insert("craft_efficiency_1", resourceful);
}

// Skill management
bool SkillTree::unlockSkill(const std::string& skillName) {
	SkillNode* skill = getSkill(skillName);
	if (skill == nullptr) {
		return false; // Skill not found
	}

	if (!skill->canUnlock()) {
		return false; // Prerequisites not met
	}

	if (fAvailablePoints < skill->getCost()) {
		return false; // Not enough skill points
	}

	skill->unlockSkill();
	spendSkillPoints(skill->getCost());
	return true;
}

bool SkillTree::levelUpSkill(const std::string& skillName) {
	SkillNode* skill = getSkill(skillName);
	if (skill == nullptr) {
		return false; // Skill not found
	}

	if (!skill->isUnlocked() || skill->isMaxLevel()) {
		return false; // Not unlocked or already max level
	}

	if (fAvailablePoints < skill->getCost()) {
		return false; // Not enough skill points
	}

	skill->levelUpSkill();
	spendSkillPoints(skill->getCost());
	return true;
}

SkillNode* SkillTree::getSkill(const std::string& skillName) const {
	SkillNode** result = fSkillLookup.search(skillName);
	if (result != nullptr) {
		return *result;
	}
	return nullptr;
}

// Skill point management
void SkillTree::addSkillPoints(int aPoints) {
	fAvailablePoints += aPoints;
}

void SkillTree::spendSkillPoints(int aPoints) {
	fAvailablePoints -= aPoints;
	if (fAvailablePoints < 0) {
		fAvailablePoints = 0;
	}
}

int SkillTree::getAvailablePoints() const {
	return fAvailablePoints;
}

// Tree operations
void SkillTree::addRootSkill(SkillNode* aSkill) {
	fRootSkills.pushBack(aSkill);
}

SinglyLinkedList<SkillNode*>& SkillTree::getRootSkills() {
	return fRootSkills;
}

// Calculate total bonuses from all unlocked skills
int SkillTree::getTotalDamageBonus() const {
	int totalDamage = 0, totalHealth = 0, totalStamina = 0, totalInfection = 0, totalCrafting = 0;
	float totalCraftingFloat = 0.0f, totalScavenge = 0.0f;
	for (auto it = fRootSkills.begin(); it != fRootSkills.end(); ++it) {
		SkillNode* rootSkill = *it;
		calculateTotalBonusesRecursive(rootSkill, totalDamage, totalHealth, totalStamina,
			totalInfection, totalCrafting, totalCraftingFloat, totalScavenge);
	}
	return totalDamage;
}

int SkillTree::getTotalHealthBonus() const {
	int totalDamage = 0, totalHealth = 0, totalStamina = 0, totalInfection = 0, totalCrafting = 0;
	float totalCraftingFloat = 0.0f, totalScavenge = 0.0f;
	for (auto it = fRootSkills.begin(); it != fRootSkills.end(); ++it) {
		SkillNode* rootSkill = *it;
		calculateTotalBonusesRecursive(rootSkill, totalDamage, totalHealth, totalStamina,
			totalInfection, totalCrafting, totalCraftingFloat, totalScavenge);
	}
	return totalHealth;
}

int SkillTree::getTotalStaminaBonus() const {
	int totalDamage = 0, totalHealth = 0, totalStamina = 0, totalInfection = 0, totalCrafting = 0;
	float totalCraftingFloat = 0.0f, totalScavenge = 0.0f;
	for (auto it = fRootSkills.begin(); it != fRootSkills.end(); ++it) {
		SkillNode* rootSkill = *it;
		calculateTotalBonusesRecursive(rootSkill, totalDamage, totalHealth, totalStamina,
			totalInfection, totalCrafting, totalCraftingFloat, totalScavenge);
	}
	return totalStamina;
}

int SkillTree::getTotalInfectionResistance() const {
	int totalDamage = 0, totalHealth = 0, totalStamina = 0, totalInfection = 0, totalCrafting = 0;
	float totalCraftingFloat = 0.0f, totalScavenge = 0.0f;
	for (auto it = fRootSkills.begin(); it != fRootSkills.end(); ++it) {
		SkillNode* rootSkill = *it;
		calculateTotalBonusesRecursive(rootSkill, totalDamage, totalHealth, totalStamina,
			totalInfection, totalCrafting, totalCraftingFloat, totalScavenge);
	}
	return totalInfection;
}

float SkillTree::getTotalCraftingBonus() const {
	int totalDamage = 0, totalHealth = 0, totalStamina = 0, totalInfection = 0, totalCrafting = 0;
	float totalCraftingFloat = 0.0f, totalScavenge = 0.0f;
	for (auto it = fRootSkills.begin(); it != fRootSkills.end(); ++it) {
		SkillNode* rootSkill = *it;
		calculateTotalBonusesRecursive(rootSkill, totalDamage, totalHealth, totalStamina,
			totalInfection, totalCrafting, totalCraftingFloat, totalScavenge);
	}
	return totalCraftingFloat;
}

float SkillTree::getTotalScavengeBonus() const {
	int totalDamage = 0, totalHealth = 0, totalStamina = 0, totalInfection = 0, totalCrafting = 0;
	float totalCraftingFloat = 0.0f, totalScavenge = 0.0f;
	for (auto it = fRootSkills.begin(); it != fRootSkills.end(); ++it) {
		SkillNode* rootSkill = *it;
		calculateTotalBonusesRecursive(rootSkill, totalDamage, totalHealth, totalStamina,
			totalInfection, totalCrafting, totalCraftingFloat, totalScavenge);
	}
	return totalScavenge;
}

// Helper recursive function to calculate total bonuses
void SkillTree::calculateTotalBonusesRecursive(SkillNode* aNode, int& totalDamage, int& totalHealth,
	int& totalStamina, int& totalInfection, int& totalCrafting, float& totalCraftingFloat, float& totalScavenge) const {
	if (aNode == nullptr || !aNode->isUnlocked()) {
		return;
	}

	// Add this node's bonuses
	totalDamage += aNode->getDamageBonus();
	totalHealth += aNode->getHealthBonus();
	totalStamina += aNode->getStaminaBonus();
	totalInfection += aNode->getInfectionResistance();
	totalCraftingFloat += aNode->getCraftingSpeedBonus();
	totalScavenge += aNode->getScavengeBonus();

	// Recursively process children
	for (auto it = aNode->getChildren().begin(); it != aNode->getChildren().end(); ++it) {
		SkillNode* child = *it;
		calculateTotalBonusesRecursive(child, totalDamage, totalHealth, totalStamina,
			totalInfection, totalCrafting, totalCraftingFloat, totalScavenge);
	}
}

// Utility
int SkillTree::getTotalUnlockedSkills() const {
	int count = 0;
	for (auto it = fRootSkills.begin(); it != fRootSkills.end(); ++it) {
		SkillNode* rootSkill = *it;
		countUnlockedRecursive(rootSkill, count);
	}
	return count;
}

void SkillTree::countUnlockedRecursive(SkillNode* aNode, int& count) const {
	if (aNode == nullptr) {
		return;
	}

	if (aNode->isUnlocked()) {
		count++;
	}

	for (auto it = aNode->getChildren().begin(); it != aNode->getChildren().end(); ++it) {
		SkillNode* child = *it;
		countUnlockedRecursive(child, count);
	}
}

bool SkillTree::hasUnlockedSkill(const std::string& skillName) const {
	SkillNode* skill = getSkill(skillName);
	if (skill != nullptr) {
		return skill->isUnlocked();
	}
	return false;
}

// Helper function to traverse tree and collect unlocked skills
void getUnlockedSkillsRecursive(SkillNode* node, std::vector<std::string>& skillIDs, std::vector<int>& levels) {
	if (node == nullptr) {
		return;
	}

	// If skill is unlocked, save its ID and level
	if (node->isUnlocked()) {
		skillIDs.push_back(node->getSkillID());
		levels.push_back(node->getLevel());
	}

	// Recursively check children
	for (auto it = node->getChildren().begin(); it != node->getChildren().end(); ++it) {
		SkillNode* child = *it;
		getUnlockedSkillsRecursive(child, skillIDs, levels);
	}
}

void SkillTree::getUnlockedSkillData(std::vector<std::string>& skillIDs, std::vector<int>& levels) const {
	skillIDs.clear();
	levels.clear();

	// Traverse all root skills and their children
	for (auto it = fRootSkills.begin(); it != fRootSkills.end(); ++it) {
		SkillNode* root = *it;
		getUnlockedSkillsRecursive(root, skillIDs, levels);
	}
}

void SkillTree::restoreUnlockedSkills(const std::vector<std::string>& skillIDs, const std::vector<int>& levels) {
	if (skillIDs.size() != levels.size()) {
		return; // Data mismatch
	}

	// Restore each skill
	for (size_t i = 0; i < skillIDs.size(); i++) {
		SkillNode* skill = getSkill(skillIDs[i]);
		if (skill != nullptr) {
			// Unlock the skill
			skill->unlockSkill();

			// Restore level (level up additional times if needed)
			int currentLevel = skill->getLevel();
			int targetLevel = levels[i];
			for (int level = currentLevel; level < targetLevel; level++) {
				skill->levelUpSkill();
			}
		}
	}
}