
// ============================================================================
// LOCATION-BASED MUSIC CONTROL
// ============================================================================

bool AudioEngine::playLocationMusic(const std::string& locationID) {
	std::string musicPath;

	// Map location IDs to music files
	if (locationID == "ruined_city" || locationID == "Ruined City") {
		musicPath = "Audio\\Music\\ruined_city.mp3";
		std::cout << "[AUDIO] Switching to Ruined City music\n";
	}
	else if (locationID == "hospital" || locationID == "Hospital") {
		musicPath = "Audio\\Music\\hospital.mp3";  // Add this file if you have it
		std::cout << "[AUDIO] Switching to Hospital music\n";
	}
	else if (locationID == "police_station" || locationID == "Police Station") {
		musicPath = "Audio\\Music\\police_station.mp3";  // Add this file if you have it
		std::cout << "[AUDIO] Switching to Police Station music\n";
	}
	else if (locationID == "shopping_mall" || locationID == "Shopping Mall") {
		musicPath = "Audio\\Music\\shopping_mall.mp3";  // Add this file if you have it
		std::cout << "[AUDIO] Switching to Shopping Mall music\n";
	}
	else if (locationID == "subway" || locationID == "Subway") {
		musicPath = "Audio\\Music\\subway.mp3";  // Add this file if you have it
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
	return stopBackgroundMusic();
}
