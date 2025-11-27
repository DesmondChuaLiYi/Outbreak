#include "Smoker.h"

// Constructor
Smoker::Smoker()
	: Zombie("smoker", "Smoker", DEFAULT_HEALTH, DEFAULT_DAMAGE, DEFAULT_SPEED, "Smoker"),
	  fHasUsedTongue(false), fTongueCooldown(0), fTongueRange(TONGUE_RANGE) {
}

// Parameterised constructor
Smoker::Smoker(const std::string& aID, const std::string& aName)
	: Zombie(aID, aName, DEFAULT_HEALTH, DEFAULT_DAMAGE, DEFAULT_SPEED, "Smoker"),
	  fHasUsedTongue(false), fTongueCooldown(0), fTongueRange(TONGUE_RANGE) {
}

// Destructor
Smoker::~Smoker() {
}

// Override methods
int Smoker::getAttackPower() const {
	return fDamage;
}

int Smoker::specialAttack() {
	return tongueAttack();
}

int Smoker::chooseAttack() {
	if (!fHasUsedTongue) {
		return tongueAttack();
	}
	return attack();
}

float Smoker::getAccuracy() const {
	return 0.70f + (fSpeed * 0.015f); // Base accuracy plus speed modifier
}

std::string Smoker::applyStatusEffect() const {
	return "Pull"; // Inflicts pull effect on hit
}

bool Smoker::isEnraged() const {
	return fHealth <= (fMaxHealth / 4); // Enraged at 25% health
}

// Smoker-specific methods
int Smoker::tongueAttack() {
	fHasUsedTongue = true;
	fTongueCooldown = 3; // Cooldown for next tongue attack
	return TONGUE_DAMAGE;
}

bool Smoker::hasUsedTongue() const {
	return fHasUsedTongue;
}

int Smoker::getTongueCooldown() const {
	return fTongueCooldown;
}

// Display information
void Smoker::displayInformation() {
	std::cout << "Name: " << fName << "\n";
	std::cout << "Health: " << fHealth << "/" << fMaxHealth << ", Damage: " << fDamage << ", Speed: " << fSpeed << "\n";
	std::cout << "Tongue Attack Damage: " << TONGUE_DAMAGE << ", Tongue Range: " << fTongueRange << "\n";
	std::cout << "Has Used Tongue: " << (fHasUsedTongue ? "Yes" : "No") << ", Tongue Cooldown: " << fTongueCooldown << "\n";
	if (isEnraged()) {
		std::cout << "STATUS: [ENRAGED]\n";
	}
}