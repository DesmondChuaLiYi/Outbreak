#include "Zombie.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

// Default constructor
Zombie::Zombie()
	: Entity(), fHealth(100), fMaxHealth(100), fDamage(10), fSpeed(5), fIsAlive(true), fType("Walker") {
}

// Parameterised constructor
Zombie::Zombie(const std::string& aID, const std::string& aName, int aHealth, int aDamage, int aSpeed, const std::string& aType)
	: Entity(aName), fHealth(aHealth), fMaxHealth(aHealth), fDamage(aDamage), fSpeed(aSpeed), fIsAlive(true), fType(aType) {
}

// Destructor
Zombie::~Zombie() {
}

// Getter methods
int Zombie::getHealth() const {
	return fHealth;
}

int Zombie::getMaxHealth() const {
	return fMaxHealth;
}

int Zombie::getDamage() const {
	return fDamage;
}

int Zombie::getSpeed() const {
	return fSpeed;
}

bool Zombie::isAlive() const {
	return fIsAlive;
}

std::string Zombie::getType() const {
	return fType;
}

// Setter methods
void Zombie::setHealth(int aHealth) {
	fHealth = aHealth;
	if (fHealth <= 0) {
		die();
	}
}

void Zombie::setDamage(int aDamage) {
	fDamage = aDamage;
}

void Zombie::setSpeed(int aSpeed) {
	fSpeed = aSpeed;
}

// Zombie methods
void Zombie::takeDamage(int aDamage) {
	fHealth -= aDamage;
	if (fHealth <= 0) {
		die();
	}
}

int Zombie::attack() {
	// Apply damage to player

	return 0;
}

void Zombie::die() {
	// Implementation here
}

// Combat behaviour methods
// To be implemented in derived classes
int Zombie::getAttackPower() const {
	return 0;
}

int Zombie::specialAttack() {
	return 0;
}

int Zombie::chooseAttack() {
	return 0;
}

float Zombie::getAccuracy() const {
	return 0.0f;
}

std::string Zombie::applyStatusEffect() const {
	return "None";
}

float Zombie::getDamageReduction() const {
	return 0.0f;
}

bool Zombie::isEnraged() const {
	return false;
}

void Zombie::onEnrage() {
	// Implementation here
}

void Zombie::displayInformation() {
	std::cout << "Zombie name: " << fName << "\n";
	std::cout << "Enemy type: " << fType << "\n";
	std::cout << "HP: " << fHealth << "/" << fMaxHealth << "\n";
}