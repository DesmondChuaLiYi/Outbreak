#include "CommonInfected.h"

// Default constructor
CommonInfected::CommonInfected()
	: Zombie("commoninfected", "Common Infected", DEFAULT_HEALTH, DEFAULT_DAMAGE, DEFAULT_SPEED, "Common Infected"),
	  fCanSwarm(true), fSwarmCount(7) {
}

// Parameterised constructor
CommonInfected::CommonInfected(const std::string& aID, const std::string& aName)
	: Zombie(aID, aName, DEFAULT_HEALTH, DEFAULT_DAMAGE, DEFAULT_SPEED, "Common Infected"),
	  fCanSwarm(true), fSwarmCount(7) {
}

// Destructor
CommonInfected::~CommonInfected() {
}

// Override methods
int CommonInfected::getAttackPower() const {
	return fDamage;
}

int CommonInfected::specialAttack() {
	return swarmAttack();
}

int CommonInfected::chooseAttack() {
	if (canSwarm()) {
		return swarmAttack();
	}
	return attack();
}

float CommonInfected::getAccuracy() const {
	return 0.75f + (fSpeed * 0.02f); // Base accuracy plus speed modifier
}

std::string CommonInfected::applyStatusEffect() const {
	return "Infection"; // Inflicts infection on hit
}

bool CommonInfected::isEnraged() const {
	return fHealth <= (fMaxHealth / 4); // Enraged at 25% health
}

// CommonInfected-specific methods
int CommonInfected::swarmAttack() {
	int totalDamage = fDamage * fSwarmCount;
	fCanSwarm = false; // Can only swarm once
	return totalDamage;
}

bool CommonInfected::canSwarm() const {
	return fCanSwarm;
}

int CommonInfected::getSwarmCount() const {
	return fSwarmCount;
}

void CommonInfected::setSwarmCount(int aCount) {
	fSwarmCount = aCount;
}

// Display information
void CommonInfected::displayInformation() {
	Zombie::displayInformation();
	std::cout << "  Can Swarm: " << (fCanSwarm ? "Yes" : "No") << "\n";
	std::cout << "  Swarm Count: " << fSwarmCount << "\n";
}