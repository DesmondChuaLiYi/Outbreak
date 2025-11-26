#include "Tank.h"

// Constructor
Tank::Tank()
	: Zombie("tank", "Tank", DEFAULT_HEALTH, DEFAULT_DAMAGE, DEFAULT_SPEED, "Tank"),
	  fPoundCooldown(0), fRockThrowCooldown(0), fCanThrowRock(1),
	  fEnrageMultiplier(1.5f), fIsEnragedState(false), fHealthThreshold(DEFAULT_HEALTH / 3) {
}

// Parameterised constructor
Tank::Tank(const std::string& aID, const std::string& aName)
	: Zombie(aID, aName, DEFAULT_HEALTH, DEFAULT_DAMAGE, DEFAULT_SPEED, "Tank"),
	  fPoundCooldown(0), fRockThrowCooldown(0), fCanThrowRock(1),
	  fEnrageMultiplier(1.5f), fIsEnragedState(false), fHealthThreshold(DEFAULT_HEALTH / 3) {
}

// Destructor
Tank::~Tank() {
}

// Override methods
int Tank::getAttackPower() const {
	return fDamage;
}

int Tank::specialAttack() {
	// Choose between pound or rock throw based on cooldowns
	if (fCanThrowRock > 0 && fRockThrowCooldown == 0) {
		return rockThrowAttack();
	}
	return poundAttack();
}

int Tank::chooseAttack() {
	// Prioritise rock throw if available
	if (fCanThrowRock > 0 && fRockThrowCooldown == 0) {
		return rockThrowAttack();
	}
	return poundAttack();
}

float Tank::getAccuracy() const {
	return 0.65f + (fSpeed * 0.01f); // Base accuracy plus speed modifier
}

std::string Tank::applyStatusEffect() const {
	return "Stun"; // Inflicts stun on hit
}

bool Tank::isEnraged() const {
	return fHealth <= fHealthThreshold; // Enraged at defined health threshold
}

void Tank::onEnrage() {
	if (!fIsEnragedState) {
		fDamage = static_cast<int>(fDamage * fEnrageMultiplier);
		fIsEnragedState = true;
	}
}

void Tank::takeDamage(int aDamage) {
	fHealth -= aDamage;
	if (fHealth <= 0) {
		die();
	}
	else if (isEnraged()) {
		onEnrage();
	}
}

// Tank-specific methods
int Tank::poundAttack() {
	fPoundCooldown = 3; // Cooldown for next pound attack
	return POUND_DAMAGE;
}

int Tank::rockThrowAttack() {
	fRockThrowCooldown = 5; // Cooldown for next rock throw
	fCanThrowRock--;
	return ROCK_THROW_DAMAGE;
}

int Tank::getPoundDamage() const {
	return POUND_DAMAGE;
}

int Tank::getRockThrowDamage() const {
	return ROCK_THROW_DAMAGE;
}

int Tank::takeAreaDamage(int aDamage) {
	// Tank takes less damage from sources like explosions
	int reducedDamage = static_cast<int>(aDamage * 0.75f); //
	takeDamage(reducedDamage);

	return reducedDamage;
}

float Tank::getEnrageMultiplier() const {
	return fEnrageMultiplier;
}

int Tank::getHealthThreshold() const {
	return fHealthThreshold;
}

// Display information
void Tank::displayInformation() {
	std::cout << "Tank Name: " << fName << "]\n";
	std::cout << "Type: " << fType << "\n";
	std::cout << "Health: " << fHealth << "/" << fMaxHealth << "\n";
	std::cout << "Damage: " << fDamage << "\n";
	std::cout << "Speed: " << fSpeed << "\n";
	std::cout << "Pound Cooldown: " << fPoundCooldown << "\n";
	std::cout << "Rock Throw Cooldown: " << fRockThrowCooldown << "\n";
	std::cout << "Can Throw Rock: " << fCanThrowRock << "\n";
	std::cout << "Enraged: " << (fIsEnragedState ? "Yes" : "No") << "\n";
}