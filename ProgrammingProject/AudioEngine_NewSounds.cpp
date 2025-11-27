
// ============================================================================
// NEW SOUND EFFECTS
// ============================================================================

void AudioEngine::playLevelUpSound() {
	if (!pLevelUpBuffer) {
		if (FAILED(loadWaveFile("Audio\\SFX\\level_up.wav", &pLevelUpBuffer))) {
			std::cout << "[SOUND] Level up (no audio file)\n";
			return;
		}
	}

	pLevelUpBuffer->SetCurrentPosition(0);
	pLevelUpBuffer->Play(0, 0, 0);
}

void AudioEngine::playSkillUnlockSound() {
	if (!pSkillUnlockBuffer) {
		if (FAILED(loadWaveFile("Audio\\SFX\\skill_unlock.wav", &pSkillUnlockBuffer))) {
			std::cout << "[SOUND] Skill unlock (no audio file)\n";
			return;
		}
	}

	pSkillUnlockBuffer->SetCurrentPosition(0);
	pSkillUnlockBuffer->Play(0, 0, 0);
}

void AudioEngine::playSaveGameSound() {
	if (!pSaveGameBuffer) {
		if (FAILED(loadWaveFile("Audio\\SFX\\save_game.wav", &pSaveGameBuffer))) {
			std::cout << "[SOUND] Save game (no audio file)\n";
			return;
		}
	}

	pSaveGameBuffer->SetCurrentPosition(0);
	pSaveGameBuffer->Play(0, 0, 0);
}
