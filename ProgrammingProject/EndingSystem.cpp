#include "EndingSystem.h"
#include "AudioEngine.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <limits> // For std::numeric_limits

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

	// Perfectionist ending: All clues but no boss (shouldn't happen in normal gameplay)
	if (hasAllClues) {
		return PERFECTIONIST_ENDING;
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
	case PERFECTIONIST_ENDING:
		displayPerfectionistEnding();
		break;
	case TRUE_ENDING:
		displayTrueEnding();
		break;
	}

	// Wait for user to press Enter to return to title screen
	std::cout << "\n\n";
	std::cout << "  Press ENTER to return to the title screen...";
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
	printAnimatedTitle("THE END", "You Have Fallen");

	std::cout << "\n\n";

	// Animated story text
	std::string story = 
		"  The darkness takes you. Your vision fades, the sounds of the infected\n"
		"  growing distant. You fought hard, survivor. You gave everything.\n\n"
		"  But in this world, sometimes everything isn't enough.\n\n"
		"  The outbreak continues. The water remains poisoned. The infected roam free.\n"
		"  Your story ends here, another name lost to the apocalypse.\n\n"
		"  Perhaps another survivor will finish what you started.\n"
		"  Perhaps they'll find the clues you left behind.\n"
		"  Perhaps they'll succeed where you fell.\n\n"
		"  Rest now, survivor. Your fight is over.";

	printAnimatedText(story);

	// Ending label
	std::cout << "\n\n";
	std::cout << "  ???????????????????????????????????????????????????????????????\n";
	std::cout << "  Ending 0/4: The Fallen (Bad Ending)\n";
	std::cout << "  ???????????????????????????????????????????????????????????????\n";
}

void EndingSystem::displayNormalEnding() {
	system("cls");
	std::cout << "\n\n";

	// Animated title
	printAnimatedTitle("SANCTUARY SECURED", "A New Beginning");

	std::cout << "\n\n";

	// Animated story text
	std::string story = 
		"  The Tank falls. The Sanctuary is yours.\n\n"
		"  You stand in the control room, hands on the water purification system.\n"
		"  The cure is here. The means to save what's left of humanity.\n\n"
		"  But as you look at the journals scattered around, the clues you missed,\n"
		"  you realize there's so much you don't know. So many questions unanswered.\n\n"
		"  Who started this? Why did they let it happen? What really caused the outbreak?\n"
		"  The truth remains buried in the locations you didn't fully explore.\n\n"
		"  Still, you've done what matters most. You've secured the Sanctuary.\n"
		"  You've given humanity a chance. That's good enough.\n\n"
		"  The world will rebuild. And maybe, just maybe, someone else will\n"
		"  uncover the full truth you couldn't find.";

	printAnimatedText(story);

	// Ending label
	std::cout << "\n\n";
	std::cout << "  ???????????????????????????????????????????????????????????????\n";
	std::cout << "  Ending 1/4: Good Enough to Keep Going (Normal Ending)\n";
	std::cout << "  ???????????????????????????????????????????????????????????????\n";
}

void EndingSystem::displayPerfectionistEnding() {
	system("cls");
	std::cout << "\n\n";

	// Animated title
	printAnimatedTitle("THE TRUTH REVEALED", "Knowledge is Power");

	std::cout << "\n\n";

	// Animated story text
	std::string story = 
		"  You've collected every clue. Read every journal. Uncovered every secret.\n\n"
		"  The water plant. The corporate negligence. The government cover-up.\n"
		"  Patient Zero. The failed containment. The lies that doomed millions.\n\n"
		"  You know the full story now. Every piece of the puzzle fits together.\n"
		"  The outbreak wasn't an accident. It was inevitable. Greed, corruption,\n"
		"  and indifference created the perfect storm.\n\n"
		"  With the Sanctuary secured and the truth in your hands, you have\n"
		"  the power to ensure this never happens again. The cure will save lives.\n"
		"  The truth will prevent future disasters.\n\n"
		"  You've done more than survive. You've become the keeper of history,\n"
		"  the guardian of truth. The world will rebuild, and it will remember.\n\n"
		"  Because you made sure of it.";

	printAnimatedText(story);

	// Ending label
	std::cout << "\n\n";
	std::cout << "  ???????????????????????????????????????????????????????????????\n";
	std::cout << "  Ending 2/4: The Archivist (Perfectionist Ending)\n";
	std::cout << "  ???????????????????????????????????????????????????????????????\n";
}

void EndingSystem::displayTrueEnding() {
	system("cls");
	std::cout << "\n\n";

	// Animated title
	printAnimatedTitle("OUTBREAK: RESOLVED", "The Complete Victory");

	std::cout << "\n\n";

	// Animated story text
	std::string story = 
		"  The Protector falls. The Sanctuary opens its doors.\n\n"
		"  You stand victorious, every clue collected, every secret uncovered,\n"
		"  every challenge overcome. You've done the impossible.\n\n"
		"  The water purification system hums to life. Clean water flows.\n"
		"  The cure spreads. The infected begin to fall. Humanity breathes again.\n\n"
		"  But you've done more than save lives. You've preserved the truth.\n"
		"  Every journal, every note, every piece of evidence—you have it all.\n\n"
		"  The corporations that caused this will face justice.\n"
		"  The government officials who covered it up will be exposed.\n"
		"  The world will know what happened, and why.\n\n"
		"  You didn't just survive the outbreak. You ended it.\n"
		"  You didn't just find the cure. You found the truth.\n"
		"  You didn't just fight. You won.\n\n"
		"  The world will rebuild. And it will do so with you as its guardian,\n"
		"  ensuring that the mistakes of the past are never repeated.\n\n"
		"  This is your legacy, survivor. This is your victory.\n"
		"  Complete. Total. Absolute.";

	printAnimatedText(story);

	// Ending label
	std::cout << "\n\n";
	std::cout << "  ???????????????????????????????????????????????????????????????\n";
	std::cout << "  Ending 3/4: The Guardian (True Ending)\n";
	std::cout << "  ???????????????????????????????????????????????????????????????\n";
}

// ============================================================================
// ANIMATION HELPER FUNCTIONS
// ============================================================================

void EndingSystem::printAnimatedText(const std::string& text) {
	for (char c : text) {
		std::cout << c << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
}

void EndingSystem::printAnimatedTitle(const std::string& title, const std::string& subtitle) {
	// Print separator
	std::cout << "  ???????????????????????????????????????????????????????????????\n";
	
	// Print title with animation
	std::cout << "  ";
	for (char c : title) {
		std::cout << c << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	std::cout << "\n";

	// Print subtitle with animation
	std::cout << "  ";
	for (char c : subtitle) {
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
		audio->playBackgroundMusic("Audio/Music/ending.wav");
	}
}
