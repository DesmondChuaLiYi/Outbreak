#ifndef TANK_H
#define TANK_H
#include "Zombie.h"

class Tank : public Zombie {
private:
	static const int DEFAULT_HEALTH = 700;   // Increased from 500
	static const int DEFAULT_DAMAGE = 35;    // Increased from 25
	static const int DEFAULT_SPEED = 2;
	static const int POUND_DAMAGE = 60;      // Increased from 40
	static const int ROCK_THROW_DAMAGE = 50; // Increased from 30

	int fPoundCooldown;
	int fRockThrowCooldown;
	int fCanThrowRock;
	float fEnrageMultiplier;
	bool fIsEnragedState;
	int fHealthThreshold;

public:
	// Constructor
	Tank();

	// Parameterised constructor
	Tank(const std::string& aID, const std::string& aName);

	// Destructor
	virtual ~Tank();

	// Override methods
	virtual int getAttackPower() const override;
	virtual int specialAttack() override;
	virtual int chooseAttack() override;
	virtual float getAccuracy() const override;
	virtual std::string applyStatusEffect() const override;
	virtual bool isEnraged() const override;
	virtual void onEnrage() override;
	virtual void takeDamage(int aDamage) override;

	// Tank-specific methods
	virtual int poundAttack();
	virtual int rockThrowAttack();
	virtual int getPoundDamage() const;
	virtual int getRockThrowDamage() const;
	virtual int takeAreaDamage(int aDamage);
	virtual float getEnrageMultiplier() const;
	virtual int getHealthThreshold() const;

	// Display information
	virtual void displayInformation() override;
};
#endif /* TANK_H */