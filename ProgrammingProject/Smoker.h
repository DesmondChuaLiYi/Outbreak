#ifndef SMOKER_H
#define SMOKER_H
#include "Zombie.h"

class Smoker : public Zombie {
private:
	static const int DEFAULT_HEALTH = 50;  // Increased from 30
	static const int DEFAULT_DAMAGE = 11;  // Increased from 8
	static const int DEFAULT_SPEED = 2;
	static const int TONGUE_DAMAGE = 20;   // Increased from 12
	static const int TONGUE_RANGE = 50;

	bool fHasUsedTongue;
	int fTongueCooldown;
	int fTongueRange;

public:
	// Constructor
	Smoker();

	// Parameterised constructor
	Smoker(const std::string& aID, const std::string& aName);

	// Destructor
	virtual ~Smoker();

	// Override methods
	virtual int getAttackPower() const override;
	virtual int specialAttack() override;
	virtual int chooseAttack() override;
	virtual float getAccuracy() const override;
	virtual std::string applyStatusEffect() const override;
	virtual bool isEnraged() const override;

	// Smoker-specific methods
	virtual int tongueAttack();
	virtual bool hasUsedTongue() const;
	virtual int getTongueCooldown() const;

	// Display information
	virtual void displayInformation() override;
};
#endif /* SMOKER_H */