#include "SkillNode.h"

// Constructor
SkillNode::SkillNode()
	: fSkillName(""), fSkillDescription(""), fType(SkillType::COMBAT),
	  fCost(0), fLevel(0), fMaxLevel(3), fUnlocked(false),
	  fDamageBonus(0), fHealthBonus(0), fStaminaBonus(0),
	  fInfectionResistance(0), fCraftingSpeedBonus(0.0f), fScavengeBonus(0.0f),
	  fParent(nullptr) {
}

// Parameterised constructor
SkillNode::SkillNode(const std::string& aID, const std::string& aName, const std::string& aDescription, SkillType aType, int aCost, int aMaxLevel)
	: fSkillName(aName), fSkillDescription(aDescription), fType(aType),
	  fCost(aCost), fLevel(0), fMaxLevel(aMaxLevel), fUnlocked(false),
	  fDamageBonus(0), fHealthBonus(0), fStaminaBonus(0),
	  fInfectionResistance(0), fCraftingSpeedBonus(0.0f), fScavengeBonus(0.0f),
	  fParent(nullptr), fSkillID(aID) {
}

// Destructor
SkillNode::~SkillNode() {
	// Clean up child nodes
	for (SkillNode* child : fChildren) {
		delete child;
	}
}

// Tree operations
void SkillNode::addChild(SkillNode* aChild) {
	fChildren.pushBack(aChild);
	aChild->setParent(this);
}

void SkillNode::setParent(SkillNode* aParent) {
	fParent = aParent;
}

SkillNode* SkillNode::getParent() const {
	return fParent;
}

SinglyLinkedList<SkillNode*>& SkillNode::getChildren() {
	return fChildren;
}

const SinglyLinkedList<SkillNode*>& SkillNode::getChildren() const {
	return fChildren;
}

// Getter methods
std::string SkillNode::getSkillName() const {
	return fSkillName;
}

std::string SkillNode::getSkillID() const {
	return fSkillID;
}

std::string SkillNode::getSkillDescription() const {
	return fSkillDescription;
}

SkillNode::SkillType SkillNode::getSkillType() const {
	return fType;
}

int SkillNode::getCost() const {
	return fCost;
}

int SkillNode::getLevel() const {
	return fLevel;
}

int SkillNode::getMaxLevel() const {
	return fMaxLevel;
}

bool SkillNode::isUnlocked() const {
	return fUnlocked;
}

// Setter methods
void SkillNode::setDamageBonus(int aBonus) {
	fDamageBonus = aBonus;
}

void SkillNode::setHealthBonus(int aBonus) {
	fHealthBonus = aBonus;
}

void SkillNode::setStaminaBonus(int aBonus) {
	fStaminaBonus = aBonus;
}

void SkillNode::setInfectionResistance(int aResistance) {
	fInfectionResistance = aResistance;
}

void SkillNode::setCraftingSpeedBonus(float aBonus) {
	fCraftingSpeedBonus = aBonus;
}

void SkillNode::setScavengeBonus(float aBonus) {
	fScavengeBonus = aBonus;
}

// Stat bonus getters
int SkillNode::getDamageBonus() const {
	return fDamageBonus;
}

int SkillNode::getHealthBonus() const {
	return fHealthBonus;
}

int SkillNode::getStaminaBonus() const {
	return fStaminaBonus;
}

int SkillNode::getInfectionResistance() const {
	return fInfectionResistance;
}

float SkillNode::getCraftingSpeedBonus() const {
	return fCraftingSpeedBonus;
}

float SkillNode::getScavengeBonus() const {
	return fScavengeBonus;
}

// Skill operations
void SkillNode::unlockSkill() {
	if (!fUnlocked) {
		fUnlocked = true;
		fLevel = 1;
	}
}

void SkillNode::levelUpSkill() {
	if (fUnlocked && fLevel < fMaxLevel) {
		++fLevel;
	}
}

bool SkillNode::canUnlock() const {
	if (fUnlocked) {
		return false; // Already unlocked
	}
	if (fParent == nullptr) {
		return true; // Root skill can be unlocked directly
	}
	return fParent->isUnlocked(); // Can unlock if parent is unlocked
}

bool SkillNode::isMaxLevel() const {
	return fLevel >= fMaxLevel;
}

// Utility
std::string SkillNode::typeToString() const {
	switch (fType) {
		case SkillType::COMBAT:
			return "Combat";
		case SkillType::SURVIVAL:
			return "Survival";
		case SkillType::CRAFTING:
			return "Crafting";
		case SkillType::MEDICAL:
			return "Medical";
		case SkillType::SCAVENGING:
			return "Scavenging";
		default:
			return "Unknown";
	}
}

void SkillNode::displaySkillInfo() const {
	std::cout << "Skill Name: " << fSkillName << "\n";
	std::cout << "Description: " << fSkillDescription << "\n";
	std::cout << "Type: " << typeToString() << "\n";
	std::cout << "Cost: " << fCost << " skill points\n";
	std::cout << "Level: " << fLevel << "/" << fMaxLevel << "\n";
	std::cout << "Unlocked: " << (fUnlocked ? "Yes" : "No") << "\n";
	std::cout << "Bonuses:\n";
	if (fDamageBonus != 0) std::cout << "  Damage Bonus: " << fDamageBonus << "\n";
	if (fHealthBonus != 0) std::cout << "  Health Bonus: " << fHealthBonus << "\n";
	if (fStaminaBonus != 0) std::cout << "  Stamina Bonus: " << fStaminaBonus << "\n";
	if (fInfectionResistance != 0) std::cout << "  Infection Resistance: " << fInfectionResistance << "\n";
	if (fCraftingSpeedBonus != 0.0f) std::cout << "  Crafting Speed Bonus: " << fCraftingSpeedBonus * 100 << "%\n";
	if (fScavengeBonus != 0.0f) std::cout << "  Scavenge Bonus: " << fScavengeBonus * 100 << "%\n";
}