#ifndef ITEM_H
#define ITEM_H
#include <string>
#include <iostream>
#include "Entity.h"

class Item : public Entity {
public:
	enum class Category {
		WEAPON, // Crowbar, Shotgun, Knife, etc
		MEDICAL, // Medkit, Bandage, Antibiotics
		FOOD, // Canned food, Water, Chocolate bars
		MATERIAL, // Cloth, Scrap metal, Wood (items for crafting)
		TOOL, // Lockpick, Gas mask, Flashlight
		KEY_ITEM, // Key, Map, Important quest items
		CONSUMABLE // One-time use items (such as grenades)
	};

	enum class EffectType {
		NONE, // No effect
		DAMAGE_BOOST, // Increased damage for _ turns
		HEALTH_REGEN, // Gradual health restoration over _ turns
		DEFENSE_BOOST, // Increased defense for _ turns
		SPEED_BOOST, // Increased speed for _ turns
		INFECTED, // Inflict infection status
		STAMINA_BOOST, // Increased stamina for _ turns
		STEALTH,
		BLEEDING
	};

private:
	Category fCategory; // Item category
	std::string fDescription; // Item description
	int fQuantity; // Item quantity
	int fInventorySpace; // How much inventory space needed
	bool fConsumable; // Is the item consumed on use?
	bool fUsable; // Can the item be used directly?

	// Instant effects (applied immediately upon use)
	int fHealthRestore; // Health restored
	int fHungerRestore; // Hunger restored
	int fInfectionCure; // Infection cured
	int fDamageBoost; // Increased attack for weapons

	// Weapon-specific stats
	int fAmmo; // Current ammo (for ranged weapons)
	int fMaxAmmo; // Max ammo capacity
	int fDurability; // Current durability
	int fMaxDurability; // Max durability

	EffectType fEffectType; // Type of effect
	int fEffectTurns; // Duration of effect in turns
	int fEffectPower; // Magnitude/strength of effect
	bool fHasBuffEffect; // Does the effect have a duration?

public:
	// Constructor
	Item();

	// Parameterised constructor
	Item(const std::string& aID, const std::string& aName, Category aCategory, const std::string& aDescription,
		int aQuantity, int aInventorySpace, bool aConsumable, bool aUsable,
		int aHealthRestore, int aHungerRestore, int aInfectionCure, int aDamageBoost,
		EffectType aEffectType = EffectType::NONE, int aEffectTurns = 0, int aEffectPower = 0);

	// Getter methods
	Category getCategory() const;
	std::string getDescription() const;
	int getQuantity() const;
	int getInventorySpace() const;
	int getTotalInventorySpace() const;
	bool isConsumable() const;
	bool isUsable() const;

	// Instant effect getters
	int getHealthRestore() const;
	int getHungerRestore() const;
	int getInfectionCure() const;
	int getDamageBoost() const;

	// Buff effect getters
	bool hasBuffEffect() const;
	EffectType getEffectType() const;
	int getEffectTurns() const;
	int getEffectPower() const;

	// Setter methods
	void setCategory(Category aCategory);
	void setDescription(const std::string& aDescription);
	void setQuantity(int aQuantity);
	void setInventorySpace(int aInventorySpace);
	void setConsumable(bool aConsumable);
	void setUsable(bool aUsable);

	// Instant effect setters
	void setHealthRestore(int aHealthRestore);
	void setHungerRestore(int aHungerRestore);
	void setInfectionCure(int aInfectionCure);
	void setDamageBoost(int aDamageBoost);

	// Buff effect setters
	void setEffectType(EffectType aEffectType);
	void setEffectTurns(int aEffectTurns);
	void setEffectPower(int aEffectPower);

	// Weapon stats getters
	int getAmmo() const;
	int getMaxAmmo() const;
	int getDurability() const;
	int getMaxDurability() const;
	float getDurabilityPercent() const;

	// Weapon stats setters
	void setAmmo(int ammo);
	void setMaxAmmo(int maxAmmo);
	void setDurability(int durability);
	void setMaxDurability(int maxDurability);
	void decreaseDurability(int amount = 1);
	bool isBroken() const;

	// Utility methods
	void addQuantity(int amount);
	void reduceQuantity(int amount);
	bool isDepleted() const;
	bool useItem(); // Returns true if item was used successfully
	std::string categoryToString(Category category) const;
	std::string effectTypeToString(EffectType effectType) const;
	
	// Comparison operators
	bool operator==(const Item& other) const;
	bool operator!=(const Item& other) const;

	// Override Talk method
	void displayInformation() override;
};

#endif /* ITEM_H */
