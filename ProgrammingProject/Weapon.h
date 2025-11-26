#ifndef WEAPON_H
#define WEAPON_H
#include "Item.h"
#include <string>

class Weapon : public Item {
public:
	enum class WeaponType {
		MELEE,
		RANGED,
		THROWABLE
	};

private:
	WeaponType fWeaponType;
	int fWeaponDamage;
	int fCurrentAmmo;
	int fMaxAmmo;
	int fDurability;
	int fMaxDurability;
	float fAccuracy;

public:
	// Default constructor
	Weapon();

	// Parameterized constructor
	Weapon(const std::string& aID, const std::string& aName, WeaponType aType,
		int aDamage, int aMaxAmmo, int aDurability, float aAccuracy, const std::string& aDescription);

	// Destructor
	virtual ~Weapon() = default;

	// ========================================================================
	// GETTERS
	// ========================================================================

	WeaponType getWeaponType() const;
	int getWeaponDamage() const;
	int getCurrentAmmo() const;
	int getMaxAmmo() const;
	int getDurability() const;
	int getMaxDurability() const;
	float getAccuracy() const;

	// ========================================================================
	// SETTERS
	// ========================================================================

	void setCurrentAmmo(int ammo);
	void addAmmo(int amount);

	// ========================================================================
	// WEAPON USE & STATUS
	// ========================================================================

	// Use weapon and consume ammo if ranged, reduce durability
	bool use();

	// Check if weapon needs ammo
	bool needsAmmo() const;

	// Check if weapon is broken
	bool isBroken() const;

	// ========================================================================
	// UTILITY METHODS
	// ========================================================================

	// Get string representation of weapon type
	std::string getTypeString() const;
};

#endif /* WEAPON_H */