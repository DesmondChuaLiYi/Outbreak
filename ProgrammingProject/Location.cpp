#include "Location.h"
#include <iostream>

// Default constructor
Location::Location()
	: Entity("", "Unknown Location"),
	fType(LocationType::CITY),
	fDescription(""),
	fAtmosphere(""),
	fVisited(false),
	fLooted(false),
	fCleared(false),
	fHazard(Hazard::NONE),
	fHazardDamage(0),
	fStoryClue(""),
	fHasClue(false),
	fClueCollected(false),
	fChapterNumber(0),
	fChapterTitle(""),
	fChapterStory("") {
}

// Parameterised constructor
Location::Location(const std::string& aID, const std::string& aName, LocationType aType,
	const std::string& aDescription, const std::string& aAtmosphere,
	Hazard aHazard, int aHazardDamage, const std::string& aStoryClue, bool aHasClue)
	: Entity(aID, aName),
	fType(aType),
	fDescription(aDescription),
	fAtmosphere(aAtmosphere),
	fVisited(false),
	fLooted(false),
	fCleared(false),
	fHazard(aHazard),
	fHazardDamage(aHazardDamage),
	fStoryClue(aStoryClue),
	fHasClue(aHasClue),
	fClueCollected(false),
	fChapterNumber(0),
	fChapterTitle(""),
	fChapterStory("") {
}

// Destructor
Location::~Location() {
	// Clean up zombie pointers
	while (hasZombies()) {
		Zombie* zombie = getNextZombie();
		delete zombie;
	}
}

// Connection management (graph edges)
void Location::addConnection(const std::string& aLocationID) {
	// Check if connection already exists
	if (!isConnectedTo(aLocationID)) {
		fConnectedLocations.pushBack(aLocationID);
	}
}

void Location::removeConnection(const std::string& aLocationID) {
	fConnectedLocations.remove(aLocationID);
}

bool Location::isConnectedTo(const std::string& aLocationID) const {
	for (auto it = fConnectedLocations.begin(); it != fConnectedLocations.end(); ++it) {
		if (*it == aLocationID) {
			return true;
		}
	}
	return false;
}

SinglyLinkedList<std::string>& Location::getConnections() {
	return fConnectedLocations;
}

int Location::getConnectionCount() const {
	return fConnectedLocations.size();
}

// Item management
void Location::addItem(const Item& aItem) {
	fItemsInLocation.pushBack(aItem);
}

bool Location::removeItem(const Item& aItem) {
	return fItemsInLocation.remove(aItem);
}

SinglyLinkedList<Item>& Location::getItems() {
	return fItemsInLocation;
}

bool Location::hasItems() const {
	return !fItemsInLocation.isEmpty();
}

void Location::lootLocation() {
	fLooted = true;
}

// Zombie management
void Location::addZombie(Zombie* aZombie) {
	if (aZombie != nullptr) {
		fZombiesInLocation.enqueue(aZombie);
	}
}

Zombie* Location::getNextZombie() {
	if (!fZombiesInLocation.isEmpty()) {
		return fZombiesInLocation.dequeue();
	}
	return nullptr;
}

bool Location::hasZombies() const {
	return !fZombiesInLocation.isEmpty();
}

int Location::getZombieCount() const {
	return fZombiesInLocation.size();
}

void Location::clearZombies() {
	fCleared = true;
}

// Hazard management
void Location::setHazard(Hazard aHazard, int aHazardDamage) {
	fHazard = aHazard;
	fHazardDamage = aHazardDamage;
}

Location::Hazard Location::getHazard() const {
	return fHazard;
}

int Location::getHazardDamage() const {
	return fHazardDamage;
}

bool Location::hasHazard() const {
	return fHazard != Hazard::NONE;
}

std::string Location::hazardToString() const {
	switch (fHazard) {
	case Hazard::NONE:
		return "None";
	case Hazard::TOXIC_FOG:
		return "Toxic Fog";
	case Hazard::ACID_RAIN:
		return "Acid Rain";
	case Hazard::COLLAPSED_FLOOR:
		return "Collapsed Floor";
	case Hazard::CONTAMINATED_WATER:
		return "Contaminated Water";
	case Hazard::DARKNESS:
		return "Darkness";
	default:
		return "Unknown";
	}
}

// Story clue management
void Location::setStoryClue(const std::string& aStoryClue) {
	fStoryClue = aStoryClue;
	fHasClue = !aStoryClue.empty();
}

std::string Location::getStoryClue() const {
	return fStoryClue;
}

bool Location::hasClue() const {
	return fHasClue && !fClueCollected;
}

void Location::collectClue() {
	fClueCollected = true;
}

bool Location::isClueCollected() const {
	return fClueCollected;
}

// Getter methods
Location::LocationType Location::getLocationType() const {
	return fType;
}

std::string Location::getDescription() const {
	return fDescription;
}

std::string Location::getAtmosphere() const {
	return fAtmosphere;
}

bool Location::isVisited() const {
	return fVisited;
}

bool Location::isLooted() const {
	return fLooted;
}

bool Location::isCleared() const {
	return fCleared;
}

// Setter methods
void Location::setDescription(const std::string& aDescription) {
	fDescription = aDescription;
}

void Location::setAtmosphere(const std::string& aAtmosphere) {
	fAtmosphere = aAtmosphere;
}

void Location::setVisited(bool aVisited) {
	fVisited = aVisited;
}

void Location::setLooted(bool aLooted) {
	fLooted = aLooted;
}

void Location::setCleared(bool aCleared) {
	fCleared = aCleared;
}

void Location::markVisited() {
	fVisited = true;
}

// Utility methods
std::string Location::typeToString() const {
	switch (fType) {
	case LocationType::CITY:
		return "City";
	case LocationType::INDUSTRIAL:
		return "Industrial";
	case LocationType::FOREST:
		return "Forest";
	case LocationType::SUBURBAN:
		return "Suburban";
	case LocationType::HOSPITAL:
		return "Hospital";
	case LocationType::MILITARY:
		return "Military";
	case LocationType::SANCTUARY:
		return "Sanctuary";
	default:
		return "Unknown";
	}
}

void Location::displayLocationInfo() const {
	std::cout << "\n=== Location Information ===\n";
	std::cout << "Name: " << getName() << "\n";
	std::cout << "Type: " << typeToString() << "\n";
	std::cout << "Description: " << fDescription << "\n";
	std::cout << "Atmosphere: " << fAtmosphere << "\n";
	std::cout << "Status: ";
	if (fVisited) std::cout << "Visited ";
	if (fLooted) std::cout << "Looted ";
	if (fCleared) std::cout << "Cleared ";
	if (!fVisited && !fLooted && !fCleared) std::cout << "Unexplored";
	std::cout << "\n";

	// Display hazard
	if (hasHazard()) {
		std::cout << "Hazard: " << hazardToString() << " (Damage: " << fHazardDamage << " per turn)\n";
	}

	// Display items
	std::cout << "Items: " << fItemsInLocation.size() << "\n";

	// Display zombies
	std::cout << "Zombies: " << getZombieCount() << "\n";

	// Display connections
	std::cout << "Connected Locations: " << getConnectionCount() << "\n";

	// Display story clue
	if (hasClue()) {
		std::cout << "[!] Story Clue Available\n";
	}
	else if (fClueCollected) {
		std::cout << "[] Story Clue Collected\n";
	}

	std::cout << "===========================\n";
}

void Location::displayConnections() const {
	std::cout << "\n=== Connected Locations ===\n";
	if (fConnectedLocations.isEmpty()) {
		std::cout << "No connections available.\n";
	}
	else {
		int count = 1;
		for (auto it = fConnectedLocations.begin(); it != fConnectedLocations.end(); ++it) {
			std::cout << count++ << ". " << *it << "\n";
		}
	}
	std::cout << "===========================\n";
}

// Override from Entity
void Location::displayInformation() {
	displayLocationInfo();
}

// Chapter management
void Location::setChapterInfo(int chapterNumber, const std::string& chapterTitle, const std::string& chapterStory) {
	fChapterNumber = chapterNumber;
	fChapterTitle = chapterTitle;
	fChapterStory = chapterStory;
}

int Location::getChapterNumber() const {
	return fChapterNumber;
}

std::string Location::getChapterTitle() const {
	return fChapterTitle;
}

std::string Location::getChapterStory() const {
	return fChapterStory;
}

void Location::displayChapterIntro() const {
	if (fChapterNumber == 0) {
		return; // No chapter assigned
	}

	std::cout << "\n";
	std::cout << std::string(120, '=') << "\n";
	std::cout << "  CHAPTER " << fChapterNumber << ": " << fChapterTitle << "\n";
	std::cout << std::string(120, '=') << "\n\n";

	// Word wrap the chapter story
	int lineWidth = 115;
	int currentPos = 0;
	std::string text = fChapterStory;

	while (currentPos < (int)text.length()) {
		int endPos = currentPos + lineWidth;
		if (endPos >= (int)text.length()) {
			endPos = (int)text.length();
		}
		else {
			// Find last space to avoid cutting words
			while (endPos > currentPos && text[endPos] != ' ' && text[endPos] != '\n') {
				endPos--;
			}
			if (endPos == currentPos) {
				endPos = currentPos + lineWidth;
			}
		}

		std::string line = text.substr(currentPos, endPos - currentPos);
		// Trim leading spaces
		while (!line.empty() && line[0] == ' ') {
			line = line.substr(1);
		}

		std::cout << "  " << line << "\n";
		currentPos = endPos;

		// Skip whitespace
		while (currentPos < (int)text.length() && (text[currentPos] == ' ' || text[currentPos] == '\n')) {
			if (text[currentPos] == '\n') {
				std::cout << "\n"; // Preserve paragraph breaks
			}
			currentPos++;
		}
	}

	std::cout << "\n" << std::string(120, '=') << "\n";
}
