#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H
#include <string>
#include <windows.h>
#include <dsound.h>
#include <mmsystem.h>
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

class AudioEngine {
private:
	// Singleton instance
	static AudioEngine* instance;

	// Private constructor (singleton pattern)
	AudioEngine();

	// DirectSound interfaces
	LPDIRECTSOUND8 pDirectSound;
	LPDIRECTSOUNDBUFFER pPrimaryBuffer;
	LPDIRECTSOUNDBUFFER pMusicBuffer;

	// Sound effect buffers
	LPDIRECTSOUNDBUFFER pMenuSelectBuffer;
	LPDIRECTSOUNDBUFFER pMenuNavigateBuffer;
	LPDIRECTSOUNDBUFFER pCombatAttackBuffer;
	LPDIRECTSOUNDBUFFER pCombatHitBuffer;
	LPDIRECTSOUNDBUFFER pCombatMissBuffer;
	LPDIRECTSOUNDBUFFER pLootPickupBuffer;
	LPDIRECTSOUNDBUFFER pZombieDeathBuffer;
	LPDIRECTSOUNDBUFFER pEnvironmentalHazardBuffer;
	LPDIRECTSOUNDBUFFER pLevelUpBuffer;
	LPDIRECTSOUNDBUFFER pSkillUnlockBuffer;
	LPDIRECTSOUNDBUFFER pSaveGameBuffer;

	// Flag to track if music is playing
	bool isMusicPlaying;
	bool usingMCI;  // True if using MCI for MP3, false if using DirectSound for WAV
	std::string currentMusicTrack;  // Track currently playing music file
	std::string pausedMusicTrack;   // Track paused for combat music
	bool inCombatMusic;              // True when combat music is playing

	// Helper functions for DirectSound
	HRESULT loadWaveFile(const std::string& filePath, LPDIRECTSOUNDBUFFER* ppBuffer);
	void releaseBuffer(LPDIRECTSOUNDBUFFER* ppBuffer);
	bool isMp3File(const std::string& filePath);
	void playMP3Sound(const std::string& filePath);  // NEW: For MP3 sound effects

public:
	// Delete copy constructor and assignment operator
	AudioEngine(const AudioEngine&) = delete;
	AudioEngine& operator=(const AudioEngine&) = delete;

	// Get singleton instance
	static AudioEngine* getInstance();

	// Play background music (DirectSound - WAV only, DirectX requirement)
	bool playBackgroundMusic(const std::string& musicFilePath = "Audio\\Music\\background_music.wav");

	// Stop background music
	bool stopBackgroundMusic();

	// Pause background music
	bool pauseBackgroundMusic();

	// Resume background music
	bool resumeBackgroundMusic();

	// Set music volume (0-10000, where 0 is max, -10000 is silent)
	bool setMusicVolume(int volume);

	// Check if music is currently playing
	bool isPlayingMusic() const;

	// NEW: Location-based music control
	bool playLocationMusic(const std::string& locationID);
	bool stopAllMusic();  // Stops all music (for returning to title screen)
	std::string getCurrentMusicTrack() const { return currentMusicTrack; }

	// NEW: Combat music system (pauses location music, resumes after)
	bool playCombatMusic();
	bool stopCombatMusic();  // Stops combat music and resumes location music
	bool isInCombatMusic() const { return inCombatMusic; }

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
	void playLevelUpSound();      // NEW
	void playSkillUnlockSound();  // NEW
	void playSaveGameSound();     // NEW

	// Cleanup and destroy singleton
	static void destroyInstance();

	// Destructor
	~AudioEngine();
};

#endif /* AUDIOENGINE_H */
