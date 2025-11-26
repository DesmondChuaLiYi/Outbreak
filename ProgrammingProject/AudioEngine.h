#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H
#include <string>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

class AudioEngine {
private:
	// Singleton instance
	static AudioEngine* instance;

	// Private constructor (singleton pattern)
	AudioEngine();

	// MP3 file path
	std::string audioFilePath;

	// Flag to track if music is playing
	bool isMusicPlaying;

public:
	// Delete copy constructor and assignment operator
	AudioEngine(const AudioEngine&) = delete;
	AudioEngine& operator=(const AudioEngine&) = delete;

	// Get singleton instance
	static AudioEngine* getInstance();

	// Play background music
	bool playBackgroundMusic(const std::string& musicFilePath = "background_music.mp3");

	// Stop background music
	bool stopBackgroundMusic();

	// Pause background music
	bool pauseBackgroundMusic();

	// Resume background music
	bool resumeBackgroundMusic();

	// Set music volume (0-1000, where 1000 is max)
	bool setMusicVolume(int volume);

	// Check if music is currently playing
	bool isPlayingMusic() const;

	// ============================================================================
	// SOUND EFFECTS - PLACEHOLDER IMPLEMENTATIONS
	// ============================================================================
	// UPDATE LOCATION: Search for these method names in the code and uncomment calls
	// playMenuSelectSound() - Call this in: NavigationMenu.cpp when user presses ENTER
	// playMenuNavigateSound() - Call this in: NavigationMenu.cpp when user presses arrow keys
	// playCombatAttackSound() - Call this in: Combat system when player attacks
	// playCombatHitSound() - Call this in: Combat system when attack connects
	// playCombatMissSound() - Call this in: Combat system when attack misses
	// playLootPickupSound() - Call this in: Location exploration when picking up loot
	// playZombieDeathSound() - Call this in: Combat system when zombie dies
	// playEnvironmentalHazardSound() - Call this in: Location when environmental damage occurs
	
	void playMenuSelectSound();
	void playMenuNavigateSound();
	void playCombatAttackSound();
	void playCombatHitSound();
	void playCombatMissSound();
	void playLootPickupSound();
	void playZombieDeathSound();
	void playEnvironmentalHazardSound();

	// Cleanup and destroy singleton
	static void destroyInstance();

	// Destructor
	~AudioEngine();
};

#endif /* AUDIOENGINE_H */
