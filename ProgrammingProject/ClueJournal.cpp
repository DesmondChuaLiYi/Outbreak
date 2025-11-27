#include "ClueJournal.h"
#include <algorithm>

// ============================================================================
// CLUE CLASS IMPLEMENTATION
// ============================================================================

// Default constructor
Clue::Clue()
	: fClueID(0), fClueName(""), fContent(""), fLocationFound(""), fPlayerEffect(""), fCollected(false) {
}

// Parameterised constructor
Clue::Clue(int aClueID, const std::string& aName, const std::string& aContent, const std::string& aLocationFound, const std::string& aPlayerEffect)
	: fClueID(aClueID), fClueName(aName), fContent(aContent), fLocationFound(aLocationFound), fPlayerEffect(aPlayerEffect), fCollected(false) {
}

// Destructor
Clue::~Clue() {
}

// Getter methods
int Clue::getClueID() const {
	return fClueID;
}

std::string Clue::getClueName() const {
	return fClueName;
}

std::string Clue::getContent() const {
	return fContent;
}

std::string Clue::getLocationFound() const {
	return fLocationFound;
}

std::string Clue::getEffect() const {
	return fPlayerEffect;
}

bool Clue::isCollected() const {
	return fCollected;
}

// Setter methods
void Clue::setCollected(bool aCollected) {
	fCollected = aCollected;
}

// Display clue
void Clue::displayClue() const {
	std::cout << "\n";
	std::cout << "================================================================\n";
	std::cout << "[Clue #" << std::setw(2) << std::setfill('0') << fClueID << "] "
		<< std::setfill(' ') << std::left << std::setw(50) << fClueName << "\n";
	std::cout << "================================================================\n";
	std::cout << "Location: " << std::left << std::setw(56) << fLocationFound << "\n";
	
	// Word wrap for content
	int lineWidth = 62;
	int currentPos = 0;
	std::string text = fContent;

	while (currentPos < (int)text.length()) {
		int endPos = std::min(currentPos + lineWidth, (int)text.length());

		// Find last space to avoid cutting words
		if (endPos < (int)text.length()) {
			while (endPos > currentPos && text[endPos] != ' ') {
				endPos--;
			}
			if (endPos == currentPos) {
				endPos = currentPos + lineWidth;
			}
		}

		std::string line = text.substr(currentPos, endPos - currentPos);
		// Trim leading spaces
		if (!line.empty() && line[0] == ' ') {
			line = line.substr(1);
		}

		std::cout << line << "\n";
		currentPos = endPos;

		while (currentPos < (int)text.length() && text[currentPos] == ' ') {
			currentPos++;
		}
	}
	
	std::cout << "Player Effect: " << std::left << std::setw(62) << fPlayerEffect << "\n";
	std::cout << "================================================================\n";
}

// Display clue in compact format
void Clue::displayClueCompact() const {
	std::cout << "[" << std::setw(2) << std::setfill('0') << fClueID << "] "
		<< std::setfill(' ') << std::left << std::setw(35) << fClueName
		<< " | " << std::left << std::setw(20) << fLocationFound << "\n";
}

// ============================================================================
// CLUE JOURNAL CLASS IMPLEMENTATION
// ============================================================================

// Default constructor
ClueJournal::ClueJournal()
	: fTotalCluesInGame(0), fCluesCollected(0) {
}

// Destructor
ClueJournal::~ClueJournal() {
}

// Add clue to journal (register as available, not collected)
void ClueJournal::addClue(const Clue& aClue) {
	fAllClues.pushBack(aClue);
	fTotalCluesInGame++;
}

// Collect clue by ID (move from available to collected)
void ClueJournal::collectClue(int aClueID) {
	DoublyLinkedList<Clue>::Iterator it = fAllClues.begin();
	while (it != fAllClues.end()) {
		if ((*it).getClueID() == aClueID) {
			if (!(*it).isCollected()) {
				Clue collectedClue = *it;
				collectedClue.setCollected(true);
				fCollectedClues.pushBack(collectedClue);
				(*it).setCollected(true);
				fCluesCollected++;
			}
			return;
		}
		++it;
	}
}

// Check if clue is in journal (registered)
bool ClueJournal::hasClue(int aClueID) const {
	DoublyLinkedList<Clue>& clues = const_cast<DoublyLinkedList<Clue>&>(fAllClues);
	DoublyLinkedList<Clue>::Iterator it = clues.begin();
	while (it != clues.end()) {
		if ((*it).getClueID() == aClueID) {
			return true;
		}
		++it;
	}
	return false;
}

// Retrieve clue by ID from all clues
Clue* ClueJournal::getClue(int aClueID) {
	DoublyLinkedList<Clue>::Iterator it = fAllClues.begin();
	while (it != fAllClues.end()) {
		if ((*it).getClueID() == aClueID) {
			return &(it.getCurrent()->value);
		}
		++it;
	}
	return nullptr;
}

// Get total collected clues
int ClueJournal::getTotalCollected() const {
	return fCluesCollected;
}

// Get total clues in game
int ClueJournal::getTotalCluesInGame() const {
	return fTotalCluesInGame;
}

// Check if specific clue is collected
bool ClueJournal::isClueCollected(int aClueID) const {
	DoublyLinkedList<Clue>& clues = const_cast<DoublyLinkedList<Clue>&>(fCollectedClues);
	DoublyLinkedList<Clue>::Iterator it = clues.begin();
	while (it != clues.end()) {
		if ((*it).getClueID() == aClueID) {
			return true;
		}
		++it;
	}
	return false;
}

// Display all collected clues
void ClueJournal::displayAllCluesCollected() const {
	std::cout << "\n";
	std::cout << "================================================================\n";
	std::cout << "                    COLLECTED CLUES JOURNAL                     \n";
	std::cout << "================================================================\n";
	
	if (fCluesCollected == 0) {
		std::cout << "No clues collected yet.\n";
		std::cout << "================================================================\n";
		return;
	}

	DoublyLinkedList<Clue>& clues = const_cast<DoublyLinkedList<Clue>&>(fCollectedClues);
	DoublyLinkedList<Clue>::Iterator it = clues.begin();
	while (it != clues.end()) {
		(*it).displayClue();
		++it;
	}
}

// Display collected clues in reverse order
void ClueJournal::displayAllCluesReverse() const {
	std::cout << "\n";
	std::cout << "================================================================\n";
	std::cout << "              COLLECTED CLUES (REVERSE ORDER)                   \n";
	std::cout << "================================================================\n";
	
	if (fCluesCollected == 0) {
		std::cout << "No clues collected yet.\n";
		std::cout << "================================================================\n";
		return;
	}

	DoublyLinkedList<Clue>& clues = const_cast<DoublyLinkedList<Clue>&>(fCollectedClues);
	int totalNodes = fCollectedClues.size();
	for (int i = totalNodes - 1; i >= 0; i--) {
		DoublyLinkedList<Clue>::Iterator it = clues.begin();
		for (int j = 0; j < i; j++) {
			++it;
		}
		(*it).displayClue();
	}
}

// Display clues found at a specific location
void ClueJournal::displayClueByLocation(const std::string& aLocation) const {
	std::cout << "\n";
	std::cout << "================================================================\n";
	std::cout << "Clues found at: " << aLocation << "\n";
	std::cout << "================================================================\n";
	
	bool found = false;
	DoublyLinkedList<Clue>& clues = const_cast<DoublyLinkedList<Clue>&>(fCollectedClues);
	DoublyLinkedList<Clue>::Iterator it = clues.begin();
	while (it != clues.end()) {
		if ((*it).getLocationFound() == aLocation) {
			(*it).displayClue();
			found = true;
		}
		++it;
	}
	
	if (!found) {
		std::cout << "No clues found at this location.\n";
	}
	std::cout << "================================================================\n";
}

// Display a summary of collected clues
void ClueJournal::displayCluesSummary() const {
	std::cout << "\n";
	std::cout << "================================================================\n";
	std::cout << "                    CLUES SUMMARY                               \n";
	std::cout << "================================================================\n";
	std::cout << "Collected: " << fCluesCollected << " / " << fTotalCluesInGame << "\n";
	std::cout << "================================================================\n";
	
	if (fCluesCollected == 0) {
		std::cout << "No clues collected yet.\n";
		std::cout << "================================================================\n";
		return;
	}

	DoublyLinkedList<Clue>& clues = const_cast<DoublyLinkedList<Clue>&>(fCollectedClues);
	DoublyLinkedList<Clue>::Iterator it = clues.begin();
	while (it != clues.end()) {
		(*it).displayClueCompact();
		++it;
	}
	std::cout << "================================================================\n";
}

// Get completion percentage
float ClueJournal::getCompletionPercentage() const {
	if (fTotalCluesInGame == 0) {
		return 0.0f;
	}
	return (static_cast<float>(fCluesCollected) / static_cast<float>(fTotalCluesInGame)) * 100.0f;
}

// Check if all clues are collected
bool ClueJournal::hasAllClues() const {
	return fCluesCollected == fTotalCluesInGame && fTotalCluesInGame > 0;
}

// Display progress
void ClueJournal::displayProgress() const {
	std::cout << "\n";
	std::cout << "================================================================\n";
	std::cout << "                    CLUE COLLECTION PROGRESS                    \n";
	std::cout << "================================================================\n";
	std::cout << "Collected Clues: " << fCluesCollected << " / " << fTotalCluesInGame << "\n";
	
	float percentage = getCompletionPercentage();
	std::cout << "Completion: " << std::fixed << std::setprecision(1) << percentage << "%\n";
	
	// Progress bar
	std::cout << "[";
	int barWidth = 50;
	int filledWidth = static_cast<int>((percentage / 100.0f) * barWidth);
	for (int i = 0; i < barWidth; i++) {
		if (i < filledWidth) {
			std::cout << "=";
		} else {
			std::cout << " ";
		}
	}
	std::cout << "]\n";
	
	if (hasAllClues()) {
		std::cout << "\nCongratulations! You have collected all clues!\n";
	}
	std::cout << "================================================================\n";
}

// Get reference to collected clues list
DoublyLinkedList<Clue>& ClueJournal::getClues() {
	return fCollectedClues;
}

// Get list of collected clue IDs for saving
std::vector<int> ClueJournal::getCollectedClueIDs() const {
	std::vector<int> ids;
	for (auto it = fCollectedClues.begin(); it != fCollectedClues.end(); ++it) {
		ids.push_back((*it).getClueID());
	}
	return ids;
}

// Restore collected clues from saved IDs
void ClueJournal::setCollectedClueIDs(const std::vector<int>& clueIDs) {
	// CRITICAL FIX: Mark clues as collected in both lists and increment counter
	for (int clueID : clueIDs) {
		// Find in all clues and mark as collected
		DoublyLinkedList<Clue>::Iterator it = fAllClues.begin();
		while (it != fAllClues.end()) {
			if ((*it).getClueID() == clueID) {
				// Mark as collected in fAllClues
				(*it).setCollected(true);
				
				// Add to fCollectedClues
				fCollectedClues.pushBack(*it);
				fCluesCollected++;
				break;
			}
			++it;
		}
	}
}