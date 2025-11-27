#include "Spitter.h"

// Default constructor
Spitter::Spitter()
	: Zombie("spitter", "Spitter", DEFAULT_HEALTH, DEFAULT_DAMAGE, DEFAULT_SPEED, "Spitter"),
	  fHasSpit(false), fSpatCooldown(0), fAcidPoolDuration(0) {
}

// Parameterised constructor
Spitter::Spitter(const std::string& aID, const std::string& aName)
	: Zombie(aID, aName, DEFAULT_HEALTH, DEFAULT_DAMAGE, DEFAULT_SPEED, "Spitter"),
	  fHasSpit(false), fSpatCooldown(0), fAcidPoolDuration(0) {
}

// Destructor
Spitter::~Spitter() {
}

// Override methods
int Spitter::getAttackPower() const {
	return fDamage;
}

int Spitter::specialAttack() {
	return spitAcidAttack();
}

int Spitter::chooseAttack() {
	if (!fHasSpit) {
		return spitAcidAttack();
	}
	return attack();
}

float Spitter::getAccuracy() const {
	return 0.70f + (fSpeed * 0.015f); // Base accuracy plus speed modifier
}

std::string Spitter::applyStatusEffect() const {
	return "Acid Burn"; // Inflicts acid burn on hit
}

bool Spitter::isEnraged() const {
	return fHealth <= (fMaxHealth / 4); // Enraged at 25% health
}

// Spitter-specific methods
int Spitter::spitAcidAttack() {
	fHasSpit = true;
	fSpatCooldown = 4; // Cooldown for next spit attack
	return ACID_DAMAGE;
}

int Spitter::acidPoolDamage() const {
	return ACID_POOL_DAMAGE;
}

bool Spitter::hasSpit() const {
	return fHasSpit;
}

int Spitter::getSpatCooldown() const {
	return fSpatCooldown;
}

// Display information
void Spitter::displayInformation() {
	std::cout << "Name: " << fName << "\n";
	std::cout << "Type: " << fType << "\n";
	std::cout << "Health: " << fHealth << "/" << fMaxHealth << "\n";
	std::cout << "Damage: " << fDamage << "\n";
	std::cout << "Speed: " << fSpeed << "\n";
	std::cout << "Has Spit: " << (fHasSpit ? "Yes" : "No") << "\n";
	std::cout << "Spat Cooldown: " << fSpatCooldown << "\n";
	std::cout << "Acid Pool Duration: " << fAcidPoolDuration << "\n";
	if (isEnraged()) {
		std::cout << "STATUS: [ENRAGED]\n";
	}
}