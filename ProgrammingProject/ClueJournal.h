#ifndef CLUEJOURNAL_H
#define CLUEJOURNAL_H
#include "DoublyLinkedList.h"
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

// Clue class
class Clue {
private:
	int fClueID; 
	std::string fClueName; // Name of lore item
	std::string fContent; // Clue text
	std::string fLocationFound; // Where player found it
	std::string fPlayerEffect; // Effect on player (story, hint, stat change, etc.)
	bool fCollected; // Has player collected this clue?

public:
	// Constructor
	Clue();

	// Parameterised constructor
	Clue(int aClueID, const std::string& aName, const std::string& aContent, const std::string& aLocationFound, const std::string& aPlayerEffect);

	// Destructor
	virtual ~Clue();

	// Getter methods
	virtual int getClueID() const;
	virtual std::string getClueName() const;
	virtual std::string getContent() const;
	virtual std::string getLocationFound() const;
	virtual bool isCollected() const;

	// Setter methods
	virtual void setCollected(bool aCollected);

	// Utility methods
	virtual void displayClue() const;
	virtual void displayClueCompact() const;
};

// Clue journal class
class ClueJournal {
private:
	DoublyLinkedList<Clue> fAllClues; // All clues in the game (registered)
	DoublyLinkedList<Clue> fCollectedClues; // Only collected clues
	int fTotalCluesInGame; // Total available clues
	int fCluesCollected; // Number of clues collected

public:
	// Constructor
	ClueJournal();

	// Destructor
	virtual ~ClueJournal();

	// Clue management methods
	virtual void addClue(const Clue& aClue); // Add clue to journal
	virtual void collectClue(int aClueID); // Mark clue as collected
	virtual bool hasClue(int aClueID) const; // Check if clue is in journal
	virtual Clue* getClue(int aClueID); // Retrieve clue by ID
	virtual int getTotalCollected() const; // Total collected clues by the player
	virtual int getTotalCluesInGame() const; // Total clues available in the game
	virtual bool isClueCollected(int aClueID) const; // Check if specific clue is collected

	// Traversal methods (using the DoublyLinkedList)
	virtual void displayAllCluesCollected() const; // Display all collected clues
	virtual void displayAllCluesReverse() const; // Display collected clues in reverse order
	virtual void displayClueByLocation(const std::string& aLocation) const; // Display clues found at a specific location
	virtual void displayCluesSummary() const; // Display a summary of collected clues

	// Statistics and progress
	virtual float getCompletionPercentage() const; // Percentage of clues collected
	virtual bool hasAllClues() const;
	virtual void displayProgress() const;

	// Internal methods
	virtual DoublyLinkedList<Clue>& getClues();

	// Save/Load support
	virtual int getCollectedClueCount() const { return fCluesCollected; }
	virtual std::vector<int> getCollectedClueIDs() const;
	virtual void setCollectedClueIDs(const std::vector<int>& clueIDs);
};

#endif /* CLUEJOURNAL_H */