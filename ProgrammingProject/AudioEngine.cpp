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

	// Auto-play background music
	playBackgroundMusic();
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

	// Read WAV header
	WAVHeader header;
	file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));

	// Verify it's a valid WAV file
	if (strncmp(header.riff, "RIFF", 4) != 0 || strncmp(header.wave, "WAVE", 4) != 0) {
		std::cerr << "[AUDIO ERROR] Invalid WAV file: " << filePath << "\n";
		file.close();
		return E_FAIL;
	}

	// Create buffer description
	WAVEFORMATEX waveFormat = {};
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = header.numChannels;
	waveFormat.nSamplesPerSec = header.sampleRate;
	waveFormat.wBitsPerSample = header.bitsPerSample;
	waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;

	DSBUFFERDESC bufferDesc = {};
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS;
	bufferDesc.dwBufferBytes = header.dataSize;
	bufferDesc.lpwfxFormat = &waveFormat;

	// Create the buffer
	HRESULT hr = pDirectSound->CreateSoundBuffer(&bufferDesc, ppBuffer, NULL);
	if (FAILED(hr)) {
		std::cerr << "[AUDIO ERROR] Failed to create sound buffer for: " << filePath << "\n";
		file.close();
		return hr;
	}

	// Lock the buffer and write the wave data
	VOID* pBuffer1;
	DWORD dwBuffer1Size;
	VOID* pBuffer2;
	DWORD dwBuffer2Size;

	hr = (*ppBuffer)->Lock(0, header.dataSize, &pBuffer1, &dwBuffer1Size, &pBuffer2, &dwBuffer2Size, 0);
	if (FAILED(hr)) {
		std::cerr << "[AUDIO ERROR] Failed to lock buffer for: " << filePath << "\n";
		(*ppBuffer)->Release();
		*ppBuffer = nullptr;
		file.close();
		return hr;
	}

	// Read audio data
	file.read(static_cast<char*>(pBuffer1), dwBuffer1Size);
	if (pBuffer2) {
		file.read(static_cast<char*>(pBuffer2), dwBuffer2Size);
	}

	(*ppBuffer)->Unlock(pBuffer1, dwBuffer1Size, pBuffer2, dwBuffer2Size);
	file.close();

	return S_OK;
}

// Play background music (supports both WAV and MP3)
bool AudioEngine::playBackgroundMusic(const std::string& musicFilePath) {
	// Don't restart if same track is already playing
	if (isMusicPlaying && currentMusicTrack == musicFilePath) {
		return true;
	}

	// Stop current music if playing
	stopBackgroundMusic();

	// Check if file is MP3 - use MCI, otherwise use DirectSound for WAV
	if (isMp3File(musicFilePath)) {
		// Use MCI for MP3 playback
		std::string command = "open \"" + musicFilePath + "\" type mpegvideo alias MediaFile";
		MCIERROR err = mciSendStringA(command.c_str(), nullptr, 0, nullptr);
		if (err != 0) {
			std::cerr << "[AUDIO ERROR] Failed to open MP3 file: " << musicFilePath << "\n";
			return false;
		}

		err = mciSendStringA("play MediaFile repeat", nullptr, 0, nullptr);
		if (err != 0) {
			std::cerr << "[AUDIO ERROR] Failed to play MP3 file\n";
			mciSendStringA("close MediaFile", nullptr, 0, nullptr);
			return false;
		}

		usingMCI = true;
		isMusicPlaying = true;
		currentMusicTrack = musicFilePath;
		std::cout << "[AUDIO] Playing MP3 background music: " << musicFilePath << "\n";
		return true;
	}
	else {
		// Use DirectSound for WAV playback
		if (!pDirectSound) {
			std::cerr << "[AUDIO ERROR] DirectSound not initialized\n";
			return false;
		}

		// Load the WAV file
		HRESULT hr = loadWaveFile(musicFilePath, &pMusicBuffer);
		if (FAILED(hr)) {
			std::cerr << "[AUDIO ERROR] Failed to load WAV file: " << musicFilePath << "\n";
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
		std::cout << "[AUDIO] Playing WAV background music: " << musicFilePath << "\n";
		return true;
	}
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
	playMP3Sound("Audio\\SFX\\menu_select.mp3");
}

void AudioEngine::playMenuNavigateSound() {
	playMP3Sound("Audio\\SFX\\menu_navigate.mp3");
}

void AudioEngine::playCombatAttackSound() {
	playMP3Sound("Audio\\SFX\\combat_attack.mp3");
}

void AudioEngine::playCombatHitSound() {
	playMP3Sound("Audio\\SFX\\combat_hit.mp3");
}

void AudioEngine::playCombatMissSound() {
	playMP3Sound("Audio\\SFX\\combat_miss.mp3");
}

void AudioEngine::playLootPickupSound() {
	playMP3Sound("Audio\\SFX\\loot_pickup.mp3");
}

void AudioEngine::playZombieDeathSound() {
	playMP3Sound("Audio\\SFX\\zombie_death.mp3");
}

void AudioEngine::playEnvironmentalHazardSound() {
	playMP3Sound("Audio\\SFX\\environmental_hazard.mp3");
}

void AudioEngine::playLevelUpSound() {
	playMP3Sound("Audio\\SFX\\level_up.mp3");
}

void AudioEngine::playSkillUnlockSound() {
	playMP3Sound("Audio\\SFX\\skill_unlock.mp3");
}

void AudioEngine::playSaveGameSound() {
	playMP3Sound("Audio\\SFX\\save_game.mp3");
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
	if (locationID == "ruined_city" || locationID == "Ruined City") {
		musicPath = "Audio\\Music\\ruined_city.mp3";
		std::cout << "[AUDIO] Switching to Ruined City music\n";
	}
	else if (locationID == "industrial" || locationID == "Industrial") {
		musicPath = "Audio\\Music\\industrial.mp3";
		std::cout << "[AUDIO] Switching to Industrial music\n";
	}
	else if (locationID == "hollow_woods" || locationID == "Hollow Woods") {
		musicPath = "Audio\\Music\\hollow_woods.mp3";
		std::cout << "[AUDIO] Switching to Hollow Woods music\n";
	}
	else if (locationID == "cemetery" || locationID == "Cemetery") {
		musicPath = "Audio\\Music\\cemetery.mp3";
		std::cout << "[AUDIO] Switching to Cemetery music\n";
	}
	else if (locationID == "old_mill" || locationID == "Old Mill") {
		musicPath = "Audio\\Music\\old_mill.mp3";
		std::cout << "[AUDIO] Switching to Old Mill music\n";
	}
	else if (locationID == "canal" || locationID == "Canal") {
		musicPath = "Audio\\Music\\canal.mp3";
		std::cout << "[AUDIO] Switching to Canal music\n";
	}
	else if (locationID == "pump_station" || locationID == "Pump Station") {
		musicPath = "Audio\\Music\\pump_station.mp3";
		std::cout << "[AUDIO] Switching to Pump Station music\n";
	}
	else if (locationID == "hospital" || locationID == "Hospital") {
		musicPath = "Audio\\Music\\hospital.mp3";
		std::cout << "[AUDIO] Switching to Hospital music\n";
	}
	else if (locationID == "police_station" || locationID == "Police Station") {
		musicPath = "Audio\\Music\\police_station.mp3";
		std::cout << "[AUDIO] Switching to Police Station music\n";
	}
	else if (locationID == "shopping_mall" || locationID == "Shopping Mall") {
		musicPath = "Audio\\Music\\shopping_mall.mp3";
		std::cout << "[AUDIO] Switching to Shopping Mall music\n";
	}
	else if (locationID == "subway" || locationID == "Subway") {
		musicPath = "Audio\\Music\\subway.mp3";
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
	bool result = playBackgroundMusic("Audio\\Music\\combat_music.mp3");
	
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
