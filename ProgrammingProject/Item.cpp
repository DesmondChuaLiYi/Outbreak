#include "Item.h"

// Constructor
Item::Item()
	: Entity(), fCategory(Category::MATERIAL), fDescription(""), fQuantity(0), fInventorySpace(1),
	fConsumable(false), fUsable(false), fHealthRestore(0), fHungerRestore(0),
	fInfectionCure(0), fDamageBoost(0), fAmmo(0), fMaxAmmo(0), fDurability(100), fMaxDurability(100),
	fEffectType(EffectType::NONE), fEffectTurns(0), fEffectPower(0), fHasBuffEffect(false) {
}

// Parameterised constructor
Item::Item(const std::string& aID, const std::string& aName, Category aCategory, const std::string& aDescription,
	int aQuantity, int aInventorySpace, bool aConsumable, bool aUsable,
	int aHealthRestore, int aHungerRestore, int aInfectionCure, int aDamageBoost,
	EffectType aEffectType, int aEffectTurns, int aEffectPower)
	: Entity(aName), fCategory(aCategory), fDescription(aDescription), fQuantity(aQuantity),
	fInventorySpace(aInventorySpace), fConsumable(aConsumable), fUsable(aUsable),
	fHealthRestore(aHealthRestore), fHungerRestore(aHungerRestore),
	fInfectionCure(aInfectionCure), fDamageBoost(aDamageBoost), fAmmo(0), fMaxAmmo(0),
	fDurability(100), fMaxDurability(100), fEffectType(aEffectType), fEffectTurns(aEffectTurns),
	fEffectPower(aEffectPower), fHasBuffEffect(aEffectTurns > 0) {
}

// Getter methods
Item::Category Item::getCategory() const {
	return fCategory;
}

std::string Item::getDescription() const {
	return fDescription;
}

int Item::getQuantity() const {
	return fQuantity;
}

int Item::getInventorySpace() const {
	return fInventorySpace;
}

int Item::getTotalInventorySpace() const {
	return fInventorySpace * fQuantity;
}

bool Item::isConsumable() const {
	return fConsumable;
}

bool Item::isUsable() const {
	return fUsable;
}

// Instant effect getters
int Item::getHealthRestore() const {
	return fHealthRestore;
}

int Item::getHungerRestore() const {
	return fHungerRestore;
}

int Item::getInfectionCure() const {
	return fInfectionCure;
}

int Item::getDamageBoost() const {
	return fDamageBoost;
}

// Duration effect getters
bool Item::hasBuffEffect() const {
	return fHasBuffEffect;
}

Item::EffectType Item::getEffectType() const {
	return fEffectType;
}

int Item::getEffectTurns() const {
	return fEffectTurns;
}

int Item::getEffectPower() const {
	return fEffectPower;
}

// Setter methods
void Item::setCategory(Category aCategory) {
	fCategory = aCategory;
}

void Item::setDescription(const std::string& aDescription) {
	fDescription = aDescription;
}

void Item::setQuantity(int aQuantity) {
	fQuantity = aQuantity;
}

void Item::setInventorySpace(int aInventorySpace) {
	fInventorySpace = aInventorySpace;
}

void Item::setConsumable(bool aConsumable) {
	fConsumable = aConsumable;
}

void Item::setUsable(bool aUsable) {
	fUsable = aUsable;
}

// Instant effect setters
void Item::setHealthRestore(int aHealthRestore) {
	fHealthRestore = aHealthRestore;
}

void Item::setHungerRestore(int aHungerRestore) {
	fHungerRestore = aHungerRestore;
}

void Item::setInfectionCure(int aInfectionCure) {
	fInfectionCure = aInfectionCure;
}

void Item::setDamageBoost(int aDamageBoost) {
	fDamageBoost = aDamageBoost;
}

// Duration effect setters
void Item::setEffectType(EffectType aEffectType) {
	fEffectType = aEffectType;
}

void Item::setEffectTurns(int aEffectTurns) {
	fEffectTurns = aEffectTurns;
	fHasBuffEffect = (aEffectTurns > 0);
}

void Item::setEffectPower(int aEffectPower) {
	fEffectPower = aEffectPower;
}

// Weapon stats getters
int Item::getAmmo() const {
	return fAmmo;
}

int Item::getMaxAmmo() const {
	return fMaxAmmo;
}

int Item::getDurability() const {
	return fDurability;
}

int Item::getMaxDurability() const {
	return fMaxDurability;
}

float Item::getDurabilityPercent() const {
	if (fMaxDurability == 0) return 0.0f;
	return (float)fDurability / fMaxDurability;
}

// Weapon stats setters
void Item::setAmmo(int ammo) {
	fAmmo = ammo;
	if (fAmmo < 0) fAmmo = 0;
	if (fAmmo > fMaxAmmo) fAmmo = fMaxAmmo;
}

void Item::setMaxAmmo(int maxAmmo) {
	fMaxAmmo = maxAmmo;
}

void Item::setDurability(int durability) {
	fDurability = durability;
	if (fDurability < 0) fDurability = 0;
}

void Item::setMaxDurability(int maxDurability) {
	fMaxDurability = maxDurability;
}

void Item::decreaseDurability(int amount) {
	fDurability -= amount;
	if (fDurability < 0) fDurability = 0;
}

bool Item::isBroken() const {
	return fDurability <= 0;
}

// Utility methods
void Item::addQuantity(int amount) {
	fQuantity += amount;
}

void Item::reduceQuantity(int amount) {
	if (amount >= fQuantity) {
		fQuantity = 0;
	}
	else {
		fQuantity -= amount;
	}
}

bool Item::isDepleted() const {
	return fQuantity <= 0;
}

bool Item::useItem() {
	if (fUsable && fQuantity > 0) {
		reduceQuantity(1);
		return true;
	}
	return false;
}

std::string Item::categoryToString(Category category) const {
	switch (category) {
	case Category::WEAPON:
		return "Weapon";
	case Category::MEDICAL:
		return "Medical";
	case Category::FOOD:
		return "Food";
	case Category::MATERIAL:
		return "Material";
	case Category::TOOL:
		return "Tool";
	case Category::KEY_ITEM:
		return "Key Item";
	case Category::CONSUMABLE:
		return "Consumable";
	default:
		return "Unknown";
	}
}

// Helper method to convert EffectType to string
std::string Item::effectTypeToString(EffectType effectType) const {
	switch (effectType) {
	case EffectType::NONE:
		return "None";
	case EffectType::DAMAGE_BOOST:
		return "Damage Boost";
	case EffectType::HEALTH_REGEN:
		return "Health Regeneration";
	case EffectType::DEFENSE_BOOST:
		return "Defense Boost";
	case EffectType::SPEED_BOOST:
		return "Speed Boost";
	case EffectType::INFECTED:
		return "Infection";
	case EffectType::STAMINA_BOOST:
		return "Stamina Boost";
	case EffectType::STEALTH:
		return "Stealth";
	case EffectType::BLEEDING:
		return "Bleeding";
	default:
		return "Unknown";
	}
}

// Comparison operators
bool Item::operator==(const Item& other) const {
	return fName == other.fName && fDescription == other.fDescription;
}

bool Item::operator!=(const Item& other) const {
	return !(*this == other);
}

// Override Talk method
void Item::displayInformation() {
	std::cout << "\n========================================\n";
	std::cout << "   Display Item Information\n";
	std::cout << "========================================\n";
	std::cout << "Name: " << fName << std::endl;
	std::cout << "Category: " << categoryToString(fCategory) << std::endl;
	std::cout << "Description: " << fDescription << std::endl;
	std::cout << "Quantity: " << fQuantity << std::endl;
	std::cout << "Inventory space: " << fInventorySpace << std::endl;
	std::cout << "Total occupied space: " << getTotalInventorySpace() << std::endl;
	std::cout << "Consumable: " << (fConsumable ? "Yes" : "No") << std::endl;
	std::cout << "Usable: " << (fUsable ? "Yes" : "No") << std::endl;

	// Display instant effects if any
	if (fHealthRestore > 0 || fHungerRestore > 0 || fInfectionCure > 0 || fDamageBoost > 0) {
		std::cout << "\nInstant Effects:\n";
		if (fHealthRestore > 0) {
			std::cout << "  - Restores " << fHealthRestore << " HP\n";
		}
		if (fHungerRestore > 0) {
			std::cout << "  - Restores " << fHungerRestore << " Hunger\n";
		}
		if (fInfectionCure > 0) {
			std::cout << "  - Cures " << fInfectionCure << "% Infection\n";
		}
		if (fDamageBoost > 0) {
			std::cout << "  - Adds " << fDamageBoost << " Damage\n";
		}
	}

	// Display buff effects if any
	if (fEffectType != EffectType::NONE) {
		std::cout << "\nBuff Effect:\n";
		std::cout << "  - Type: " << effectTypeToString(fEffectType) << std::endl;
		std::cout << "  - Duration: " << fEffectTurns << " turns\n";
		std::cout << "  - Power: " << fEffectPower << std::endl;
	}

	std::cout << "========================================\n\n";
}