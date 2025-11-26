#ifndef SPITTER_H
#define SPITTER_H
#include "Zombie.h"

class Spitter : public Zombie {
private:
	static const int DEFAULT_HEALTH = 45;  // Increased from 30
	static const int DEFAULT_DAMAGE = 10;  // Increased from 6
	static const int DEFAULT_SPEED = 3;
	static const int ACID_DAMAGE = 18;     // Increased from 10
	static const int ACID_POOL_DAMAGE = 12; // Increased from 5

	bool fHasSpit;
	int fSpatCooldown;
	int fAcidPoolDuration;

public:
	// Constructor
	Spitter();

	// Parameterised constructor
	Spitter(const std::string& aID, const std::string& aName);

	// Destructor
	virtual ~Spitter();

	// Override methods
	virtual int getAttackPower() const override;
	virtual int specialAttack() override;
	virtual int chooseAttack() override;
	virtual float getAccuracy() const override;
	virtual std::string applyStatusEffect() const override;
	virtual bool isEnraged() const override;

	// Spitter-specific methods
	virtual int spitAcidAttack();
	virtual int acidPoolDamage() const;
	virtual bool hasSpit() const;
	virtual int getSpatCooldown() const;

	// Display information
	virtual void displayInformation() override;
};

#endif /* SPITTER_H */