#ifndef COMMONINFECTED_H
#define COMMONINFECTED_H
#include "Zombie.h"

class CommonInfected : public Zombie {
private:
	static const int DEFAULT_HEALTH = 25;  // Increased from 15
	static const int DEFAULT_DAMAGE = 7;   // Increased from 4
	static const int DEFAULT_SPEED = 3;    // Increased from 2

	bool fCanSwarm;
	int fSwarmCount;

public:
	// Constructor
	CommonInfected();

	// Parameterised constructor
	CommonInfected(const std::string& aID, const std::string& aName);

	// Destructor
	virtual ~CommonInfected();

	// Override methods
	virtual int getAttackPower() const override;
	virtual int specialAttack() override;
	virtual int chooseAttack() override;
	virtual float getAccuracy() const override;
	virtual std::string applyStatusEffect() const override;
	virtual bool isEnraged() const override;

	// CommonInfected-specific methods
	virtual int swarmAttack();
	virtual bool canSwarm() const;
	virtual int getSwarmCount() const;
	virtual void setSwarmCount(int aCount);

	// Display information
	virtual void displayInformation() override;
};

#endif /* COMMONINFECTED_H */