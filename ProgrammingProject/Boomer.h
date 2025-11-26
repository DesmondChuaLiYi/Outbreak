#ifndef BOOMER_H
#define BOOMER_H
#include "Zombie.h"

class Boomer : public Zombie {
private:
	static const int DEFAULT_HEALTH = 70;   // Increased from 50
	static const int DEFAULT_DAMAGE = 12; // Increased from 8
	static const int DEFAULT_SPEED = 2;
	static const int VOMIT_DAMAGE = 22;     // Increased from 15
	static const int EXPLOSION_DAMAGE = 35; // Increased from 20

	bool fHasVomited;
	int fVomitCooldown;

public:
	// Constructor
	Boomer();

	// Parameterised constructor
	Boomer(const std::string& aID, const std::string& aName);

	// Destructor
	virtual ~Boomer();

	// Override methods
	virtual int getAttackPower() const override;
	virtual int specialAttack() override;
	virtual int chooseAttack() override;
	virtual float getAccuracy() const override;
	virtual std::string applyStatusEffect() const override;
	virtual bool isEnraged() const override;

	// Boomer-specific methods
	virtual int vomitAttack();
	virtual int explosionAttack();
	virtual bool hasVomited() const;
	virtual int getVomitCooldown() const;

	// Phase 2: Special Abilities
	virtual bool hasSpecialAbility() const override { return true; }
	virtual int getSpecialAbilityChance() const override { return 100; } // Always explodes on death
	virtual void onDeath(class Player* target) override;
	virtual std::string getSpecialAbilityName() const override { return "Explosion"; }

	// Display information
	virtual void displayInformation() override;
};
#endif /* BOOMER_H */