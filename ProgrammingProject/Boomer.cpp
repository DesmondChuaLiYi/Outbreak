#include "Boomer.h"
#include "Player.h"

// Default constructor
Boomer::Boomer()
	: Zombie("boomer", "Boomer", DEFAULT_HEALTH, DEFAULT_DAMAGE, DEFAULT_SPEED, "Boomer"),
	  fHasVomited(false), fVomitCooldown(0) {
}

// Parameterised constructor
Boomer::Boomer(const std::string& aID, const std::string& aName)
	: Zombie(aID, aName, DEFAULT_HEALTH, DEFAULT_DAMAGE, DEFAULT_SPEED, "Boomer"),
	  fHasVomited(false), fVomitCooldown(0) {
}

// Destructor
Boomer::~Boomer() {
}

// Override methods
int Boomer::getAttackPower() const {
	return fDamage;
}

int Boomer::specialAttack() {
	return vomitAttack();
}

int Boomer::chooseAttack() {
	if (!fHasVomited) {
		return vomitAttack();
	}
	return attack();
}

float Boomer::getAccuracy() const {
	return 0.60f + (fSpeed * 0.01f); // Base accuracy plus speed modifier
}

std::string Boomer::applyStatusEffect() const {
	return "Blinded"; // Inflicts blindness on hit
}

bool Boomer::isEnraged() const {
	return fHealth <= (fMaxHealth / 3); // Enraged at 33% health
}

// Boomer-specific methods
int Boomer::vomitAttack() {
	fHasVomited = true;
	fVomitCooldown = 3; // Cooldown for next vomit attack
	return VOMIT_DAMAGE;
}

int Boomer::explosionAttack() {
	return EXPLOSION_DAMAGE;
}

bool Boomer::hasVomited() const {
	return fHasVomited;
}

int Boomer::getVomitCooldown() const {
	return fVomitCooldown;
}

// Display information
void Boomer::displayInformation() {
	std::cout << "Name: " << fName << "\n";
	std::cout << "Type: " << fType << "\n";
	std::cout << "Health: " << fHealth << "/" << fMaxHealth << "\n";
	std::cout << "Damage: " << fDamage << "\n";
	std::cout << "Speed: " << fSpeed << "\n";
	std::cout << "Has Vomited: " << (fHasVomited ? "Yes" : "No") << "\n";
	std::cout << "Vomit Cooldown: " << fVomitCooldown << "\n";
}

// Phase 2: Special Ability - Explosion on Death
void Boomer::onDeath(Player* target) {
	if (target != nullptr) {
		target->takeDamage(EXPLOSION_DAMAGE);
	}
}