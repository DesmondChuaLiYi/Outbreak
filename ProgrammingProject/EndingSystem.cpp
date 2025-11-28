#include "EndingSystem.h"
#include "AudioEngine.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <limits>

// ============================================================================
// ENDING DETERMINATION LOGIC
// ============================================================================

EndingSystem::EndingType EndingSystem::determineEnding(bool playerDefeated, bool bossDefeated, Player* player, ClueJournal* journal) {
	// Bad ending: Player died
	if (playerDefeated) {
		return BAD_ENDING;
	}

	// Check if player has all clues
	bool hasAllClues = journal->hasAllClues();

	// True ending: Boss defeated + all clues collected
	if (bossDefeated && hasAllClues) {
		return TRUE_ENDING;
	}

	// Normal ending: Won but missing clues
	return NORMAL_ENDING;
}

// ============================================================================
// MAIN ENDING DISPLAY
// ============================================================================

void EndingSystem::displayEnding(EndingType type, Player* player, ClueJournal* journal) {
	// Play ending music
	playEndingMusic();

	// Display the appropriate ending
	switch (type) {
	case BAD_ENDING:
		displayBadEnding();
		break;
	case NORMAL_ENDING:
		displayNormalEnding();
		break;
	case TRUE_ENDING:
		displayTrueEnding();
		break;
	}

	// Wait for user to press ENTER to return to title screen
	std::cout << "\n\n  Press ENTER to return to the title screen...";
	std::cin.ignore();
	std::cin.get();
}

// ============================================================================
// INDIVIDUAL ENDING IMPLEMENTATIONS
// ============================================================================

void EndingSystem::displayBadEnding() {
	system("cls");
	std::cout << "\n\n";

	// Animated title
	printAnimatedTitle("THE END");

	std::cout << "\n\n";

	// Animated story text
	std::string story = 
		"  Tony Redgrave never saw the final blow coming — the world rarely gives\n"
		"  warnings before it takes what little you have left. As he fell into the\n"
		"  dirt, feeling the warmth of his blood mixing with the cold mud beneath\n"
		"  him, he realized how terrifyingly thin the line between living and dying\n"
		"  truly was.\n\n"
		"  The infected didn't roar in victory. They didn't celebrate. They simply\n"
		"  consumed, the same way the outbreak consumed every city, every family,\n"
		"  every hope the world once held.\n\n"
		"  In his last flicker of consciousness, Tony understood the lesson the\n"
		"  apocalypse had been screaming all along: in a world this broken, you\n"
		"  don't get to hesitate. Every step, every choice, every scrap of vigilance\n"
		"  is the difference between survival and extinction.\n\n"
		"  The world did not end because monsters rose —\n"
		"  it ended because people underestimated how fast the darkness spreads\n"
		"  when no one stands against it.\n\n"
		"  His story was not one of failure, but of an unfinished fight — a reminder\n"
		"  carved into the ruins for anyone still breathing:\n\n"
		"  In the end, you don't lose when the world ends.\n"
		"  You lose the moment you stop fighting to rebuild it.";

	printAnimatedText(story);
	printEndingCredits(0, "The Price of a Single Mistake");
}

void EndingSystem::displayNormalEnding() {
	system("cls");
	std::cout << "\n\n";

	// Animated title
	printAnimatedTitle("SANCTUARY SECURED");

	std::cout << "\n\n";

	// Animated story text
	std::string story = 
		"  The Tank falls. The Sanctuary is yours.\n\n"
		"  You stand in the control room, hands on the water purification system.\n"
		"  The cure is here. The means to save what's left of humanity.\n\n"
		"  But as you look at the journals scattered around, the clues you missed,\n"
		"  you realize there's so much you don't know. So many questions unanswered.\n\n"
		"  Who started this? Why did they let it happen? What really caused the\n"
		"  outbreak? The truth remains buried in the locations you didn't fully\n"
		"  explore.\n\n"
		"  Still, you've done what matters most. You've secured the Sanctuary.\n"
		"  You've given humanity a chance. That's good enough.\n\n"
		"  The water flows. The people drink. The world will rebuild. And maybe,\n"
		"  just maybe, someone else will uncover the full truth you couldn't find.\n\n"
		"  You did your part. That's all anyone can ask.";

	printAnimatedText(story);
	printEndingCredits(1, "Good Enough to Keep Going");
}

void EndingSystem::displayTrueEnding() {
	system("cls");
	std::cout << "\n\n";

	// Animated title
	printAnimatedTitle("OUTBREAK: RESOLVED");

	std::cout << "\n\n";

	// Animated story text
	std::string story = 
		"  The Protector falls. The Sanctuary opens its doors.\n\n"
		"  You stand victorious, every clue collected, every secret uncovered,\n"
		"  every challenge overcome. You've done the impossible.\n\n"
		"  The water purification system hums to life. Clean water flows.\n"
		"The cure spreads. The infected begin to fall. Humanity breathes again.\n\n"
		"  But you've done more than save lives. You've preserved the truth.\n"
		"  Every journal, every note, every piece of evidence — you have it all.\n\n"
		"  The corporations that caused this will face justice.\n"
		"  The government officials who covered it up will be exposed.\n"
		"  The world will know what happened, and why.\n\n"
		"  You didn't just survive the outbreak. You ended it.\n"
		"  You didn't just find the cure. You found the truth.\n"
		"  You didn't just fight. You won.\n\n"
		"  The world will rebuild. And it will do so with you as its guardian,\n"
		"  ensuring that the mistakes of the past are never repeated.\n\n"
		"  This is your legacy. This is your victory. Complete. Total. Absolute.";

	printAnimatedText(story);
	printEndingCredits(2, "The Return of Dante (True Ending)");
}

// ============================================================================
// ANIMATION HELPER FUNCTIONS
// ============================================================================

void EndingSystem::printAnimatedText(const std::string& text) {
	for (char c : text) {
		std::cout << c << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(15));
	}
}

void EndingSystem::printAnimatedTitle(const std::string& title) {
	// Print separator
	std::cout << "  ???????????????????????????????????????????????????????????????\n";
	
	// Print title with animation
	std::cout << "  ";
	for (char c : title) {
		std::cout << c << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	std::cout << "\n";

	// Print separator
	std::cout << "  ???????????????????????????????????????????????????????????????\n";
}

void EndingSystem::playEndingMusic() {
	AudioEngine* audio = AudioEngine::getInstance();
	if (audio != nullptr) {
		audio->stopAllMusic();
		audio->playBackgroundMusic("Audio\\Music\\ending.wav");
	}
}

void EndingSystem::printEndingCredits(int endingNumber, const std::string& endingTitle) {
	// Print ending label centered
	std::cout << "\n\n";
	std::cout << "  ???????????????????????????????????????????????????????????????\n";
	std::cout << "  Ending (" << (endingNumber + 1) << "/3): " << endingTitle << "\n";
	std::cout << "  \n";
	std::cout << "  Thank you for playing OUTBREAK\n";
	std::cout << "  ???????????????????????????????????????????????????????????????\n";
}
