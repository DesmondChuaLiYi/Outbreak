#ifndef LOCATION_H
#define LOCATION_H
#include "Entity.h"
#include "SinglyLinkedList.h"
#include "Queue.h"
#include "Item.h"
#include "Zombie.h"
#include <string>

class Location : public Entity {
public:
	enum class LocationType {
		CITY,
		INDUSTRIAL,
		FOREST,
		SUBURBAN,
		HOSPITAL,
		MILITARY,
		SANCTUARY
	};

	enum class Hazard {
		NONE,
		TOXIC_FOG,
		ACID_RAIN,
		COLLAPSED_FLOOR,
		CONTAMINATED_WATER,
		DARKNESS
	};

private:
	LocationType fType;
	std::string fDescription;
	std::string fAtmosphere;

	bool fVisited;
	bool fLooted;
	bool fCleared; // All zombies defeated

	// Graph connections (bidirectional edges)
	SinglyLinkedList<std::string> fConnectedLocations; // IDs of connected locations

	// Location contents
	SinglyLinkedList<Item> fItemsInLocation;
	Queue<Zombie*> fZombiesInLocation;

	// Hazards and conditions
	Hazard fHazard;
	int fHazardDamage; // Damage per turn from hazard

	// Story and lore
	std::string fStoryClue;
	bool fHasClue;
	bool fClueCollected;

	// Chapter information
	int fChapterNumber;
	std::string fChapterTitle;
	std::string fChapterStory;

public:
	// Constructors
	Location();

	// Parameterised constructor
	Location(const std::string& aID, const std::string& aName, LocationType aType, const std::string& aDescription,
		const std::string& aAtmosphere, Hazard aHazard = Hazard::NONE, int aHazardDamage = 0,
		const std::string& aStoryClue = "", bool aHasClue = false);

	// Destructor
	virtual ~Location();

	// Connection management (graph edges)
	void addConnection(const std::string& aLocationID);
	void removeConnection(const std::string& aLocationID);
	bool isConnectedTo(const std::string& aLocationID) const;
	SinglyLinkedList<std::string>& getConnections();
	int getConnectionCount() const;

	// Item management
	void addItem(const Item& aItem);
	bool removeItem(const Item& aItem);
	SinglyLinkedList<Item>& getItems();
	bool hasItems() const;
	void lootLocation(); // Mark as looted

	// Zombie management
	void addZombie(Zombie* aZombie);
	Zombie* getNextZombie();
	bool hasZombies() const;
	int getZombieCount() const;
	void clearZombies(); // Mark as cleared

	// Hazard management
	void setHazard(Hazard aHazard, int aHazardDamage);
	Hazard getHazard() const;
	int getHazardDamage() const;
	bool hasHazard() const;
	std::string hazardToString() const;

	// Story clue management
	void setStoryClue(const std::string& aStoryClue);
	std::string getStoryClue() const;
	bool hasClue() const;
	void collectClue();
	bool isClueCollected() const;

	// Chapter management
	void setChapterInfo(int chapterNumber, const std::string& chapterTitle, const std::string& chapterStory);
	int getChapterNumber() const;
	std::string getChapterTitle() const;
	std::string getChapterStory() const;
	void displayChapterIntro() const;

	// Getter methods
	LocationType getLocationType() const;
	std::string getDescription() const;
	std::string getAtmosphere() const;
	bool isVisited() const;
	bool isLooted() const;
	bool isCleared() const;

	// Setter methods
	void setDescription(const std::string& aDescription);
	void setAtmosphere(const std::string& aAtmosphere);
	void setVisited(bool aVisited);
	void setLooted(bool aLooted);
	void setCleared(bool aCleared);
	void markVisited();

	// Utility methods
	std::string typeToString() const;
	void displayLocationInfo() const;
	void displayConnections() const;

	// Display information
	void displayInformation() override;
};

#endif /* LOCATION_H */