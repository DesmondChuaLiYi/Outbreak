#include "Entity.h"

// Default constructor
Entity::Entity() : fID(""), fName("") {}

// Parameterized constructor (name only)
Entity::Entity(const std::string& n) : fID(""), fName(n) {}

// Parameterized constructor (ID and name)
Entity::Entity(const std::string& id, const std::string& n) : fID(id), fName(n) {}

// Destructor
Entity::~Entity() {}

// Get entity ID
std::string Entity::getID() const {
	return fID;
}

// Get entity name
std::string Entity::getName() const {
	return fName;
}

// Entity talk method
void Entity::displayInformation() {
	std::cout << fName << " is being described." << std::endl;
}
