#include "Weapon.h"

// ============================================================================
// CONSTRUCTORS
// ============================================================================

Weapon::Weapon()
	: Item(),
	fWeaponType(WeaponType::MELEE),
	fWeaponDamage(10),
	fCurrentAmmo(0),
	fMaxAmmo(0),
	fDurability(100),
	fMaxDurability(100),
	fAccuracy(1.0f) {
}

Weapon::Weapon(const std::string& aID, const std::string& aName, WeaponType aType,
	int aDamage, int aMaxAmmo, int aDurability, float aAccuracy, const std::string& aDescription)
	: Item(aID, aName, Item::Category::WEAPON, aDescription, 1, 4, false, true, 0, 0, 0, aDamage),
	fWeaponType(aType),
	fWeaponDamage(aDamage),
	fCurrentAmmo(aMaxAmmo),
	fMaxAmmo(aMaxAmmo),
	fDurability(aDurability),
	fMaxDurability(aDurability),
	fAccuracy(aAccuracy) {
}

// ============================================================================
// GETTERS
// ============================================================================

Weapon::WeaponType Weapon::getWeaponType() const {
	return fWeaponType;
}

int Weapon::getWeaponDamage() const {
	return fWeaponDamage;
}

int Weapon::getCurrentAmmo() const {
	return fCurrentAmmo;
}

int Weapon::getMaxAmmo() const {
	return fMaxAmmo;
}

int Weapon::getDurability() const {
	return fDurability;
}

int Weapon::getMaxDurability() const {
	return fMaxDurability;
}

float Weapon::getAccuracy() const {
	return fAccuracy;
}

// ============================================================================
// SETTERS
// ============================================================================

void Weapon::setCurrentAmmo(int ammo) {
	fCurrentAmmo = ammo;
}

void Weapon::addAmmo(int amount) {
	fCurrentAmmo += amount;
	if (fCurrentAmmo > fMaxAmmo) {
		fCurrentAmmo = fMaxAmmo;
	}
}

// ============================================================================
// WEAPON USE & STATUS
// ============================================================================

bool Weapon::use() {
	if (fWeaponType == WeaponType::RANGED) {
		if (fCurrentAmmo > 0) {
			fCurrentAmmo--;
			fDurability--;
			return true;
		}
		return false;
	}
	else {
		fDurability--;
		return true;
	}
}

bool Weapon::needsAmmo() const {
	return fWeaponType == WeaponType::RANGED;
}

bool Weapon::isBroken() const {
	return fDurability <= 0;
}

// ============================================================================
// UTILITY METHODS
// ============================================================================

std::string Weapon::getTypeString() const {
	switch (fWeaponType) {
	case WeaponType::MELEE:
		return "Melee";
	case WeaponType::RANGED:
		return "Ranged";
	case WeaponType::THROWABLE:
		return "Throwable";
	default:
		return "Unknown";
	}
}
