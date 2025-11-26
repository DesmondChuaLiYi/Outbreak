#ifndef ZOMBIE_H
#define ZOMBIE_H
#include "Entity.h"

class Zombie : public Entity {
protected:
	int fHealth;
	int fMaxHealth;
	int fDamage;
	int fSpeed;
	bool fIsAlive;
	std::string fType;

public:
	// Constructor
	Zombie();

	// Parameterised constructor
	Zombie(const std::string& aID, const std::string& aName, int aHealth, int aDamage, int aSpeed, const std::string& aType);

	// Destructor
	virtual ~Zombie();

	// Getter methods
	virtual int getHealth() const;
	virtual int getMaxHealth() const;
	virtual int getDamage() const;
	virtual int getSpeed() const;
	virtual bool isAlive() const;
	virtual std::string getType() const;

	// Setter methods
	virtual void setHealth(int aHealth);
	virtual void setDamage(int aDamage);
	virtual void setSpeed(int aSpeed);

	// Zombie methods
	virtual void takeDamage(int aDamage);
	virtual int attack(); // Deal damage
	virtual void die(); // Handle death

	// Combat behaviour (method to be overridden)
	virtual int getAttackPower() const;
	virtual int specialAttack(); // Special attack based on type
	virtual int chooseAttack(); // Choose between normal and special attack
	virtual float getAccuracy() const; // Accuracy based on type and speed (zombies can miss)
	virtual std::string applyStatusEffect() const;
	virtual float getDamageReduction() const; // Zombies can have damage resistance based on type
	virtual bool isEnraged() const; // Zombies get stronger on low health
	virtual void onEnrage();

	// Phase 2: Special Abilities
	virtual bool hasSpecialAbility() const { return false; }
	virtual int getSpecialAbilityChance() const { return 0; }
	virtual std::string useSpecialAbility(class Player* target) { return ""; }
	virtual void onDeath(class Player* target) {} // Triggered when zombie dies
	virtual bool canUseSpecialAbility() const { return hasSpecialAbility(); }
	virtual std::string getSpecialAbilityName() const { return "None"; }

	// Display information
	virtual void displayInformation() override;
};

#endif /* ZOMBIE_H */