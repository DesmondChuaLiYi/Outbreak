#include "AudioEngine.h"
#include <iostream>
#include <fstream>

// Initialize static member
AudioEngine* AudioEngine::instance = nullptr;

// WAV file header structure
struct WAVHeader {
	char riff[4];           // "RIFF"
	DWORD fileSize;         // File size - 8
	char wave[4];           // "WAVE"
	char fmt[4];            // "fmt "
	DWORD fmtSize;          // Format chunk size
	WORD audioFormat;       // Audio format (1 = PCM)
	WORD numChannels;       // Number of channels
	DWORD sampleRate;       // Sample rate
	DWORD byteRate;         // Byte rate
	WORD blockAlign;        // Block align
	WORD bitsPerSample;     // Bits per sample
	char data[4];           // "data"
	DWORD dataSize;         // Data size
};

// Constructor
AudioEngine::AudioEngine()
	: pDirectSound(nullptr),
	  pPrimaryBuffer(nullptr),
	  pMusicBuffer(nullptr),
	  pMenuSelectBuffer(nullptr),
	  pMenuNavigateBuffer(nullptr),
	  pCombatAttackBuffer(nullptr),
	  pCombatHitBuffer(nullptr),
	  pCombatMissBuffer(nullptr),
	  pLootPickupBuffer(nullptr),
	  pZombieDeathBuffer(nullptr),
	  pEnvironmentalHazardBuffer(nullptr),
	  pLevelUpBuffer(nullptr),
	  pSkillUnlockBuffer(nullptr),
	  pSaveGameBuffer(nullptr),
	  isMusicPlaying(false),
	  usingMCI(false),
	  inCombatMusic(false) {

	// Initialize DirectSound
	HRESULT hr = DirectSoundCreate8(NULL, &pDirectSound, NULL);
	if (FAILED(hr)) {
		std::cerr << "[AUDIO ERROR] Failed to create DirectSound device\n";
		return;
	}

	// Set cooperative level (DSSCL_PRIORITY allows format changes)
	HWND hwnd = GetConsoleWindow();
	if (hwnd == NULL) {
		hwnd = GetForegroundWindow();
	}

	hr = pDirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(hr)) {
		std::cerr << "[AUDIO ERROR] Failed to set cooperative level\n";
		pDirectSound->Release();
		pDirectSound = nullptr;
		return;
	}

	// Create primary buffer
	DSBUFFERDESC bufferDesc = {};
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

	hr = pDirectSound->CreateSoundBuffer(&bufferDesc, &pPrimaryBuffer, NULL);
	if (FAILED(hr)) {
		std::cerr << "[AUDIO ERROR] Failed to create primary buffer\n";
		pDirectSound->Release();
		pDirectSound = nullptr;
		return;
	}

	std::cout << "[AUDIO] DirectSound initialized successfully\n";
	// Note: Music will be started by main.cpp when showing title screen
}

// Destructor
AudioEngine::~AudioEngine() {
	stopBackgroundMusic();

	// Release all sound effect buffers
	releaseBuffer(&pMenuSelectBuffer);
	releaseBuffer(&pMenuNavigateBuffer);
	releaseBuffer(&pCombatAttackBuffer);
	releaseBuffer(&pCombatHitBuffer);
	releaseBuffer(&pCombatMissBuffer);
	releaseBuffer(&pLootPickupBuffer);
	releaseBuffer(&pZombieDeathBuffer);
	releaseBuffer(&pEnvironmentalHazardBuffer);
	releaseBuffer(&pLevelUpBuffer);
	releaseBuffer(&pSkillUnlockBuffer);
	releaseBuffer(&pSaveGameBuffer);

	// Release primary buffer
	if (pPrimaryBuffer) {
		pPrimaryBuffer->Release();
		pPrimaryBuffer = nullptr;
	}

	// Release DirectSound
	if (pDirectSound) {
		pDirectSound->Release();
		pDirectSound = nullptr;
	}
}

// Get singleton instance
AudioEngine* AudioEngine::getInstance() {
	if (instance == nullptr) {
		instance = new AudioEngine();
	}
	return instance;
}

// Helper function to release a buffer
void AudioEngine::releaseBuffer(LPDIRECTSOUNDBUFFER* ppBuffer) {
	if (ppBuffer && *ppBuffer) {
		(*ppBuffer)->Stop();
		(*ppBuffer)->Release();
		*ppBuffer = nullptr;
	}
}

// Helper function to check if file is MP3
bool AudioEngine::isMp3File(const std::string& filePath) {
	if (filePath.length() < 4) return false;
	std::string ext = filePath.substr(filePath.length() - 4);
	// Convert to lowercase for comparison
	for (char& c : ext) {
		c = tolower(c);
	}
	return ext == ".mp3";
}

// Helper function to play MP3 sound effects (one-shot playback)
void AudioEngine::playMP3Sound(const std::string& filePath) {
	// Generate unique alias for this sound
	static int soundCounter = 0;
	std::string alias = "SFX_" + std::to_string(soundCounter++);
	
	// Open the MP3 file
	std::string openCmd = "open \"" + filePath + "\" type mpegvideo alias " + alias;
	MCIERROR err = mciSendStringA(openCmd.c_str(), nullptr, 0, nullptr);
	if (err != 0) {
		std::cout << "[AUDIO ERROR] Failed to open MP3 sound: " << filePath << "\n";
		return;
	}
	
	// Play the sound (not looping)
	std::string playCmd = "play " + alias;
	err = mciSendStringA(playCmd.c_str(), nullptr, 0, nullptr);
	if (err != 0) {
		std::cout << "[AUDIO ERROR] Failed to play MP3 sound\n";
		mciSendStringA(("close " + alias).c_str(), nullptr, 0, nullptr);
		return;
	}
	
	// Note: Sound will auto-close when finished. For better cleanup, we could track aliases
	// and close them manually, but for short sound effects this is acceptable.
}

// Helper function to load WAV file into DirectSound buffer
HRESULT AudioEngine::loadWaveFile(const std::string& filePath, LPDIRECTSOUNDBUFFER* ppBuffer) {
	if (!pDirectSound) {
		return E_FAIL;
	}

	// Open WAV file
	std::ifstream file(filePath, std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "[AUDIO ERROR] Failed to open file: " << filePath << "\n";
		return E_FAIL;
	}

	// Read RIFF header
	char riffHeader[4];
	DWORD fileSize;
	char waveHeader[4];

	file.read(riffHeader, 4);
	file.read(reinterpret_cast<char*>(&fileSize), 4);
	file.read(waveHeader, 4);

	// Verify RIFF and WAVE
	if (strncmp(riffHeader, "RIFF", 4) != 0 || strncmp(waveHeader, "WAVE", 4) != 0) {
		std::cerr << "[AUDIO ERROR] Not a valid WAV file: " << filePath << "\n";
		file.close();
		return E_FAIL;
	}

	// Find the fmt chunk
	WAVEFORMATEX waveFormat = {};
	bool fmtFound = false;

	while (!fmtFound && file.good()) {
		char chunkID[4];
		DWORD chunkSize;

		file.read(chunkID, 4);
		file.read(reinterpret_cast<char*>(&chunkSize), 4);

		if (strncmp(chunkID, "fmt ", 4) == 0) {
			// Read format chunk
			WORD audioFormat;
			WORD numChannels;
			DWORD sampleRate;
			DWORD byteRate;
			WORD blockAlign;
			WORD bitsPerSample;

			file.read(reinterpret_cast<char*>(&audioFormat), 2);
			file.read(reinterpret_cast<char*>(&numChannels), 2);
			file.read(reinterpret_cast<char*>(&sampleRate), 4);
			file.read(reinterpret_cast<char*>(&byteRate), 4);
			file.read(reinterpret_cast<char*>(&blockAlign), 2);
			file.read(reinterpret_cast<char*>(&bitsPerSample), 2);

			// Skip any extra format bytes
			if (chunkSize > 16) {
				file.seekg(chunkSize - 16, std::ios::cur);
			}

			// Fill WAVEFORMATEX
			waveFormat.wFormatTag = audioFormat;
			waveFormat.nChannels = numChannels;
			waveFormat.nSamplesPerSec = sampleRate;
			waveFormat.nAvgBytesPerSec = byteRate;
			waveFormat.nBlockAlign = blockAlign;
			waveFormat.wBitsPerSample = bitsPerSample;
			waveFormat.cbSize = 0;

			fmtFound = true;

			std::cout << "[AUDIO DEBUG] WAV Format: " << numChannels << " channels, "
			          << sampleRate << " Hz, " << bitsPerSample << " bits\n";
		} else {
			// Skip unknown chunk
			file.seekg(chunkSize, std::ios::cur);
		}
	}

	if (!fmtFound) {
		std::cerr << "[AUDIO ERROR] No fmt chunk found in WAV file\n";
		file.close();
		return E_FAIL;
	}

	// Find the data chunk
	DWORD dataSize = 0;
	bool dataFound = false;

	while (!dataFound && file.good()) {
		char chunkID[4];
		DWORD chunkSize;

		file.read(chunkID, 4);
		file.read(reinterpret_cast<char*>(&chunkSize), 4);

		if (strncmp(chunkID, "data", 4) == 0) {
			dataSize = chunkSize;
			dataFound = true;
			std::cout << "[AUDIO DEBUG] Data chunk size: " << dataSize << " bytes\n";
		} else {
			// Skip unknown chunk
			file.seekg(chunkSize, std::ios::cur);
		}
	}

	if (!dataFound || dataSize == 0) {
		std::cerr << "[AUDIO ERROR] No data chunk found in WAV file\n";
		file.close();
		return E_FAIL;
	}

	// Create buffer description
	DSBUFFERDESC bufferDesc = {};
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS;
	bufferDesc.dwBufferBytes = dataSize;
	bufferDesc.lpwfxFormat = &waveFormat;

	// Create the buffer
	HRESULT hr = pDirectSound->CreateSoundBuffer(&bufferDesc, ppBuffer, NULL);
	if (FAILED(hr)) {
		std::cerr << "[AUDIO ERROR] Failed to create sound buffer (HRESULT: " << hr << ")\n";
		file.close();
		return hr;
	}

	// Lock the buffer and write the wave data
	VOID* pBuffer1;
	DWORD dwBuffer1Size;
	VOID* pBuffer2;
	DWORD dwBuffer2Size;

	hr = (*ppBuffer)->Lock(0, dataSize, &pBuffer1, &dwBuffer1Size, &pBuffer2, &dwBuffer2Size, 0);
	if (FAILED(hr)) {
		std::cerr << "[AUDIO ERROR] Failed to lock buffer\n";
		(*ppBuffer)->Release();
		*ppBuffer = nullptr;
		file.close();
		return hr;
	}

	// Read audio data
	file.read(static_cast<char*>(pBuffer1), dwBuffer1Size);
	if (pBuffer2 && dwBuffer2Size > 0) {
		file.read(static_cast<char*>(pBuffer2), dwBuffer2Size);
	}

	(*ppBuffer)->Unlock(pBuffer1, dwBuffer1Size, pBuffer2, dwBuffer2Size);
	file.close();

	std::cout << "[AUDIO DEBUG] Successfully loaded WAV file: " << filePath << "\n";
	return S_OK;
}

// Play background music (DirectSound for WAV files)
bool AudioEngine::playBackgroundMusic(const std::string& musicFilePath) {
	// Don't restart if same track is already playing
	if (isMusicPlaying && currentMusicTrack == musicFilePath) {
		return true;
	}

	// Stop current music if playing
	stopBackgroundMusic();

	// Use DirectSound for WAV playback (DirectX requirement)
	if (!pDirectSound) {
		std::cerr << "[AUDIO ERROR] DirectSound not initialized\n";
		return false;
	}

	// Load the WAV file
	HRESULT hr = loadWaveFile(musicFilePath, &pMusicBuffer);
	if (FAILED(hr)) {
		std::cerr << "[AUDIO ERROR] Failed to load WAV file: " << musicFilePath << "\n";
		std::cerr << "[AUDIO ERROR] Make sure file exists and is in PCM WAV format (44.1kHz, 16-bit)\n";
		return false;
	}

	// Play the music in a loop
	hr = pMusicBuffer->Play(0, 0, DSBPLAY_LOOPING);
	if (FAILED(hr)) {
		std::cerr << "[AUDIO ERROR] Failed to play WAV music\n";
		releaseBuffer(&pMusicBuffer);
		return false;
	}

	usingMCI = false;
	isMusicPlaying = true;
	currentMusicTrack = musicFilePath;
	std::cout << "[AUDIO] Now playing: " << musicFilePath << " (DirectSound)\n";
	return true;
}

// Stop background music
bool AudioEngine::stopBackgroundMusic() {
	if (usingMCI && isMusicPlaying) {
		// Stop MCI playback
		mciSendStringA("stop MediaFile", nullptr, 0, nullptr);
		mciSendStringA("close MediaFile", nullptr, 0, nullptr);
		isMusicPlaying = false;
		usingMCI = false;
		currentMusicTrack = "";
		std::cout << "[AUDIO] Background music stopped (MCI)\n";
		return true;
	}
	else if (pMusicBuffer) {
		// Stop DirectSound playback
		pMusicBuffer->Stop();
		releaseBuffer(&pMusicBuffer);
		isMusicPlaying = false;
		currentMusicTrack = "";
		std::cout << "[AUDIO] Background music stopped (DirectSound)\n";
		return true;
	}
	return false;
}

// Pause background music
bool AudioEngine::pauseBackgroundMusic() {
	if (usingMCI && isMusicPlaying) {
		mciSendStringA("pause MediaFile", nullptr, 0, nullptr);
		std::cout << "[AUDIO] Background music paused (MCI)\n";
		return true;
	}
	else if (pMusicBuffer && isMusicPlaying) {
		pMusicBuffer->Stop();
		std::cout << "[AUDIO] Background music paused (DirectSound)\n";
		return true;
	}
	return false;
}

// Resume background music
bool AudioEngine::resumeBackgroundMusic() {
	if (usingMCI) {
		mciSendStringA("resume MediaFile", nullptr, 0, nullptr);
		std::cout << "[AUDIO] Background music resumed (MCI)\n";
		return true;
	}
	else if (pMusicBuffer) {
		HRESULT hr = pMusicBuffer->Play(0, 0, DSBPLAY_LOOPING);
		if (SUCCEEDED(hr)) {
			std::cout << "[AUDIO] Background music resumed (DirectSound)\n";
			return true;
		}
	}
	return false;
}

// Set music volume
bool AudioEngine::setMusicVolume(int volume) {
	if (usingMCI) {
		// MCI volume range is 0-1000
		if (volume < 0) volume = 0;
		if (volume > 1000) volume = 1000;
		std::string command = "setaudio MediaFile volume to " + std::to_string(volume);
		mciSendStringA(command.c_str(), nullptr, 0, nullptr);
		return true;
	}
	else if (pMusicBuffer) {
		// DirectSound volume range is -10000 to 0 (where 0 is max)
		if (volume < -10000) volume = -10000;
		if (volume > 0) volume = 0;
		HRESULT hr = pMusicBuffer->SetVolume(volume);
		return SUCCEEDED(hr);
	}
	return false;
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
// SOUND EFFECT IMPLEMENTATIONS
// ============================================================================

void AudioEngine::playMenuSelectSound() {
	if (!pMenuSelectBuffer) {
		if (FAILED(loadWaveFile("Audio\\SFX\\menu_select.wav", &pMenuSelectBuffer))) {
			return;
		}
	}
	pMenuSelectBuffer->SetCurrentPosition(0);
	pMenuSelectBuffer->Play(0, 0, 0);
}

void AudioEngine::playMenuNavigateSound() {
	if (!pMenuNavigateBuffer) {
		if (FAILED(loadWaveFile("Audio\\SFX\\menu_navigate.wav", &pMenuNavigateBuffer))) {
			return;
		}
	}
	pMenuNavigateBuffer->SetCurrentPosition(0);
	pMenuNavigateBuffer->Play(0, 0, 0);
}

void AudioEngine::playCombatAttackSound() {
	if (!pCombatAttackBuffer) {
		if (FAILED(loadWaveFile("Audio\\SFX\\combat_attack.wav", &pCombatAttackBuffer))) {
			return;
		}
	}
	pCombatAttackBuffer->SetCurrentPosition(0);
	pCombatAttackBuffer->Play(0, 0, 0);
}

void AudioEngine::playCombatHitSound() {
	if (!pCombatHitBuffer) {
		if (FAILED(loadWaveFile("Audio\\SFX\\combat_hit.wav", &pCombatHitBuffer))) {
			return;
		}
	}
	pCombatHitBuffer->SetCurrentPosition(0);
	pCombatHitBuffer->Play(0, 0, 0);
}

void AudioEngine::playCombatMissSound() {
	if (!pCombatMissBuffer) {
		if (FAILED(loadWaveFile("Audio\\SFX\\combat_miss.wav", &pCombatMissBuffer))) {
			return;
		}
	}
	pCombatMissBuffer->SetCurrentPosition(0);
	pCombatMissBuffer->Play(0, 0, 0);
}

void AudioEngine::playLootPickupSound() {
	if (!pLootPickupBuffer) {
		if (FAILED(loadWaveFile("Audio\\SFX\\loot_pickup.wav", &pLootPickupBuffer))) {
			return;
		}
	}
	pLootPickupBuffer->SetCurrentPosition(0);
	pLootPickupBuffer->Play(0, 0, 0);
}

void AudioEngine::playZombieDeathSound() {
	if (!pZombieDeathBuffer) {
		if (FAILED(loadWaveFile("Audio\\SFX\\zombie_death.wav", &pZombieDeathBuffer))) {
			return;
		}
	}
	pZombieDeathBuffer->SetCurrentPosition(0);
	pZombieDeathBuffer->Play(0, 0, 0);
}

void AudioEngine::playEnvironmentalHazardSound() {
	if (!pEnvironmentalHazardBuffer) {
		if (FAILED(loadWaveFile("Audio\\SFX\\environmental_hazard.wav", &pEnvironmentalHazardBuffer))) {
			return;
		}
	}
	pEnvironmentalHazardBuffer->SetCurrentPosition(0);
	pEnvironmentalHazardBuffer->Play(0, 0, 0);
}

void AudioEngine::playLevelUpSound() {
	if (!pLevelUpBuffer) {
		if (FAILED(loadWaveFile("Audio\\SFX\\level_up.wav", &pLevelUpBuffer))) {
			return;
		}
	}
	pLevelUpBuffer->SetCurrentPosition(0);
	pLevelUpBuffer->Play(0, 0, 0);
}

void AudioEngine::playSkillUnlockSound() {
	if (!pSkillUnlockBuffer) {
		if (FAILED(loadWaveFile("Audio\\SFX\\skill_unlock.wav", &pSkillUnlockBuffer))) {
			return;
		}
	}
	pSkillUnlockBuffer->SetCurrentPosition(0);
	pSkillUnlockBuffer->Play(0, 0, 0);
}

void AudioEngine::playSaveGameSound() {
	if (!pSaveGameBuffer) {
		if (FAILED(loadWaveFile("Audio\\SFX\\save_game.wav", &pSaveGameBuffer))) {
			return;
		}
	}
	pSaveGameBuffer->SetCurrentPosition(0);
	pSaveGameBuffer->Play(0, 0, 0);
}

// ============================================================================
// LOCATION-BASED MUSIC CONTROL
// ============================================================================

bool AudioEngine::playLocationMusic(const std::string& locationID) {
	// Don't switch if in combat music
	if (inCombatMusic) {
		std::cout << "[AUDIO] Combat music playing - location music will resume after combat\n";
		return false;
	}

	std::string musicPath;

	// Map location IDs to music files
	if (locationID == "loc_ruined_city" || locationID == "ruined_city" || locationID == "Ruined City") {
		musicPath = "Audio\\Music\\ruined_city.wav";
		std::cout << "[AUDIO] Switching to Ruined City music\n";
	}
	else if (locationID == "industrial" || locationID == "Industrial") {
		musicPath = "Audio\\Music\\industrial.wav";
		std::cout << "[AUDIO] Switching to Industrial music\n";
	}
	else if (locationID == "hollow_woods" || locationID == "Hollow Woods") {
		musicPath = "Audio\\Music\\hollow_woods.wav";
		std::cout << "[AUDIO] Switching to Hollow Woods music\n";
	}
	else if (locationID == "cemetery" || locationID == "Cemetery") {
		musicPath = "Audio\\Music\\cemetery.wav";
		std::cout << "[AUDIO] Switching to Cemetery music\n";
	}
	else if (locationID == "old_mill" || locationID == "Old Mill") {
		musicPath = "Audio\\Music\\old_mill.wav";
		std::cout << "[AUDIO] Switching to Old Mill music\n";
	}
	else if (locationID == "canal" || locationID == "Canal") {
		musicPath = "Audio\\Music\\canal.wav";
		std::cout << "[AUDIO] Switching to Canal music\n";
	}
	else if (locationID == "pump_station" || locationID == "Pump Station") {
		musicPath = "Audio\\Music\\pump_station.wav";
		std::cout << "[AUDIO] Switching to Pump Station music\n";
	}
	else if (locationID == "hospital" || locationID == "Hospital") {
		musicPath = "Audio\\Music\\hospital.wav";
		std::cout << "[AUDIO] Switching to Hospital music\n";
	}
	else if (locationID == "police_station" || locationID == "Police Station") {
		musicPath = "Audio\\Music\\police_station.wav";
		std::cout << "[AUDIO] Switching to Police Station music\n";
	}
	else if (locationID == "shopping_mall" || locationID == "Shopping Mall") {
		musicPath = "Audio\\Music\\shopping_mall.wav";
		std::cout << "[AUDIO] Switching to Shopping Mall music\n";
	}
	else if (locationID == "subway" || locationID == "Subway") {
		musicPath = "Audio\\Music\\subway.wav";
		std::cout << "[AUDIO] Switching to Subway music\n";
	}
	else {
		// Default/unknown location - no music or use generic exploration music
		std::cout << "[AUDIO] No specific music for location: " << locationID << "\n";
		return false;
	}

	// Play the location-specific music
	return playBackgroundMusic(musicPath);
}

bool AudioEngine::stopAllMusic() {
	std::cout << "[AUDIO] Stopping all music (returning to title screen)\n";
	pausedMusicTrack = "";
	inCombatMusic = false;
	return stopBackgroundMusic();
}

// ============================================================================
// COMBAT MUSIC SYSTEM
// ============================================================================

bool AudioEngine::playCombatMusic() {
	if (inCombatMusic) {
		return true;  // Already in combat music
	}

	// Save current track to resume later
	pausedMusicTrack = currentMusicTrack;
	std::cout << "[AUDIO] Pausing location music: " << pausedMusicTrack << "\n";

	// Stop current music
	stopBackgroundMusic();

	// Play combat music
	inCombatMusic = true;
	bool result = playBackgroundMusic("Audio\\Music\\combat_music.wav");
	
	if (result) {
		std::cout << "[AUDIO] Combat music started!\n";
	}
	
	return result;
}

bool AudioEngine::stopCombatMusic() {
	if (!inCombatMusic) {
		return true;  // Not in combat music
	}

	std::cout << "[AUDIO] Combat ended - resuming location music\n";

	// Stop combat music
	stopBackgroundMusic();
	inCombatMusic = false;

	// Resume paused music if any
	if (!pausedMusicTrack.empty()) {
		std::cout << "[AUDIO] Resuming: " << pausedMusicTrack << "\n";
		bool result = playBackgroundMusic(pausedMusicTrack);
		pausedMusicTrack = "";
		return result;
	}

	return true;
}
