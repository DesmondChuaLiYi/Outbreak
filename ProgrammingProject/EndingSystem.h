#ifndef ENDINGSYSTEM_H
#define ENDINGSYSTEM_H

#include <string>
#include "Player.h"
#include "ClueJournal.h"

class EndingSystem {
public:
	enum EndingType {
		BAD_ENDING = 0,           // Player died
		NORMAL_ENDING = 1,      // Boss defeated but not all clues collected
		PERFECTIONIST_ENDING = 2, // All clues collected but boss not defeated
		TRUE_ENDING = 3  // Boss defeated AND all clues collected
	};

	// Determine which ending should be triggered
	static EndingType determineEnding(bool playerDefeated, bool bossDefeated, Player* player, ClueJournal* journal);

	// Display the appropriate ending
	static void displayEnding(EndingType type, Player* player, ClueJournal* journal);

private:
	// Individual ending implementations
	static void displayBadEnding();
	static void displayNormalEnding();
	static void displayPerfectionistEnding();
	static void displayTrueEnding();

	// Animation helper functions
	static void printAnimatedText(const std::string& text);
	static void printAnimatedTitle(const std::string& title, const std::string& subtitle);
	static void playEndingMusic();
};

#endif /* ENDINGSYSTEM_H */
