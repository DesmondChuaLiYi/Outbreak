#include "AudioEngine.h"
#include <iostream>

// Initialize static member
AudioEngine* AudioEngine::instance = nullptr;

// Constructor
AudioEngine::AudioEngine() : isMusicPlaying(false) {
	// Private constructor for singleton
}

// Destructor
AudioEngine::~AudioEngine() {
	stopBackgroundMusic();
}

// Get singleton instance
AudioEngine* AudioEngine::getInstance() {
	if (instance == nullptr) {
		instance = new AudioEngine();
	}
	return instance;
}

// Play background music
bool AudioEngine::playBackgroundMusic(const std::string& musicFilePath) {
	audioFilePath = musicFilePath;
	isMusicPlaying = true;

	// Use Windows multimedia to play music
	std::string command = "open \"" + musicFilePath + "\" type mpegvideo alias MediaFile";
	mciSendStringA(command.c_str(), nullptr, 0, nullptr);
	mciSendStringA("play MediaFile repeat", nullptr, 0, nullptr);

	std::cout << "[AUDIO] Playing background music: " << musicFilePath << "\n";
	return true;
}

// Stop background music
bool AudioEngine::stopBackgroundMusic() {
	if (isMusicPlaying) {
		mciSendStringA("stop MediaFile", nullptr, 0, nullptr);
		mciSendStringA("close MediaFile", nullptr, 0, nullptr);
		isMusicPlaying = false;
		std::cout << "[AUDIO] Background music stopped.\n";
	}
	return true;
}

// Pause background music
bool AudioEngine::pauseBackgroundMusic() {
	if (isMusicPlaying) {
		mciSendStringA("pause MediaFile", nullptr, 0, nullptr);
		std::cout << "[AUDIO] Background music paused.\n";
		return true;
	}
	return false;
}

// Resume background music
bool AudioEngine::resumeBackgroundMusic() {
	if (isMusicPlaying) {
		mciSendStringA("resume MediaFile", nullptr, 0, nullptr);
		std::cout << "[AUDIO] Background music resumed.\n";
		return true;
	}
	return false;
}

// Set music volume
bool AudioEngine::setMusicVolume(int volume) {
	if (volume < 0 || volume > 1000) {
		std::cerr << "[ERROR] Volume must be between 0 and 1000.\n";
		return false;
	}

	// MCI volume range is 0-1000
	std::string command = "setaudio MediaFile volume to " + std::to_string(volume);
	mciSendStringA(command.c_str(), nullptr, 0, nullptr);
	return true;
}

// Check if music is currently playing
bool AudioEngine::isPlayingMusic() const {
	return isMusicPlaying;
}

// Cleanup and destroy singleton
void AudioEngine::destroyInstance() {
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}

// ============================================================================
// SOUND EFFECT IMPLEMENTATIONS - PLACEHOLDERS
// ============================================================================

void AudioEngine::playMenuSelectSound() {
	// PLACEHOLDER: Menu select sound effect
	// TODO: Replace with actual sound file path
	// Example: mciSendStringA("open \"menu_select.wav\" type waveaudio alias MenuSelect");
	// Example: mciSendStringA("play MenuSelect", nullptr, 0, nullptr);
	std::cout << "[SOUND] Menu select sound (placeholder)\n";
}

void AudioEngine::playMenuNavigateSound() {
	// PLACEHOLDER: Menu navigate sound effect
	// TODO: Replace with actual sound file path
	// Example: mciSendStringA("open \"menu_navigate.wav\" type waveaudio alias MenuNavigate");
	// Example: mciSendStringA("play MenuNavigate", nullptr, 0, nullptr);
	std::cout << "[SOUND] Menu navigate sound (placeholder)\n";
}

void AudioEngine::playCombatAttackSound() {
	// PLACEHOLDER: Combat attack sound effect
	// TODO: Replace with actual sound file path
	// Example: mciSendStringA("open \"combat_attack.wav\" type waveaudio alias CombatAttack");
	// Example: mciSendStringA("play CombatAttack", nullptr, 0, nullptr);
	std::cout << "[SOUND] Combat attack sound (placeholder)\n";
}

void AudioEngine::playCombatHitSound() {
	// PLACEHOLDER: Combat hit sound effect
	// TODO: Replace with actual sound file path
	// Example: mciSendStringA("open \"combat_hit.wav\" type waveaudio alias CombatHit");
	// Example: mciSendStringA("play CombatHit", nullptr, 0, nullptr);
	std::cout << "[SOUND] Combat hit sound (placeholder)\n";
}

void AudioEngine::playCombatMissSound() {
	// PLACEHOLDER: Combat miss sound effect
	// TODO: Replace with actual sound file path
	// Example: mciSendStringA("open \"combat_miss.wav\" type waveaudio alias CombatMiss");
	// Example: mciSendStringA("play CombatMiss", nullptr, 0, nullptr);
	std::cout << "[SOUND] Combat miss sound (placeholder)\n";
}

void AudioEngine::playLootPickupSound() {
	// PLACEHOLDER: Loot pickup sound effect
	// TODO: Replace with actual sound file path
	// Example: mciSendStringA("open \"loot_pickup.wav\" type waveaudio alias LootPickup");
	// Example: mciSendStringA("play LootPickup", nullptr, 0, nullptr);
	std::cout << "[SOUND] Loot pickup sound (placeholder)\n";
}

void AudioEngine::playZombieDeathSound() {
	// PLACEHOLDER: Zombie death sound effect
	// TODO: Replace with actual sound file path
	// Example: mciSendStringA("open \"zombie_death.wav\" type waveaudio alias ZombieDeath");
	// Example: mciSendStringA("play ZombieDeath", nullptr, 0, nullptr);
	std::cout << "[SOUND] Zombie death sound (placeholder)\n";
}

void AudioEngine::playEnvironmentalHazardSound() {
	// PLACEHOLDER: Environmental hazard sound effect
	// TODO: Replace with actual sound file path
	// Example: mciSendStringA("open \"environmental_hazard.wav\" type waveaudio alias EnvironmentalHazard");
	// Example: mciSendStringA("play EnvironmentalHazard", nullptr, 0, nullptr);
	std::cout << "[SOUND] Environmental hazard sound (placeholder)\n";
}
