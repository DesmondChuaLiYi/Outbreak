# Combat History Stack - Manual Implementation Guide

## ⚠️ IMPORTANT
GameplayEngine.h file editing causes corruption with automated tools.
**You must add these changes manually in Visual Studio.**

---

## Step 1: Edit GameplayEngine.h

### Add Stack Include (Line 10)
**Location**: After `#include "Queue.h"` (line 9)

**Add this line**:
```cpp
#include "Stack.h"
```

**Result should look like**:
```cpp
#include "ClueJournal.h"
#include "Queue.h"
#include "Stack.h"          // ADD THIS LINE
#include "Crafting.h"
```

### Add Stack Members (Line 86)
**Location**: After `int maxWavesPerLocation;` (line 85)

**Add these lines**:
```cpp
Stack<std::string> combatActionHistory;
static const int MAX_COMBAT_HISTORY = 5;
```

**Result should look like**:
```cpp
// Combat state
Queue<Zombie*> currentWave;
int currentWaveNumber;
int maxWavesPerLocation;
Stack<std::string> combatActionHistory;      // ADD THIS
static const int MAX_COMBAT_HISTORY = 5;     // ADD THIS
```

---

## Step 2: Edit GameplayEngine.cpp - Display History

### Location: conductCombat() method, after line 533

**After this line**:
```cpp
std::cout << "\n  COMBAT\n\n";
```

**Add this code**:
```cpp
// Display combat history
if (!combatActionHistory.isEmpty()) {
    std::cout << "  RECENT ACTIONS:\n";
    Stack<std::string> tempStack;
    
    // Copy to temp (reverses order)
    while (!combatActionHistory.isEmpty()) {
        tempStack.push(combatActionHistory.pop());
    }
    
    // Display and restore
    int count = 1;
    while (!tempStack.isEmpty() && count <= MAX_COMBAT_HISTORY) {
        std::string action = tempStack.pop();
        std::cout << "  [" << count << "] " << action << "\n";
        combatActionHistory.push(action);  // Restore
        count++;
    }
    std::cout << "\n";
}
```

---

## Step 3: Track Combat Actions

### Track Damage Dealt (After line 617)

**After this code**:
```cpp
currentZombie->takeDamage(damage);
result.playerDamageDealt += damage;
```

**Add**:
```cpp
// Track action
combatActionHistory.push("Dealt " + std::to_string(damage) + " dmg to " + currentZombie->getType());
```

### Track Damage Taken (After line 633)

**After this code**:
```cpp
currentPlayer->takeDamage(specialDmg);
result.playerDamageTaken += specialDmg;
```

**Add**:
```cpp
// Track damage taken
combatActionHistory.push("Took " + std::to_string(specialDmg) + " dmg from " + currentZombie->getType());
```

### Track Zombie Kills (After line 540)

**After this code**:
```cpp
std::cout << "  [KILL] " << currentZombie->getType() << " defeated!\n";
```

**Add**:
```cpp
// Track kill
combatActionHistory.push("Killed " + currentZombie->getType());
```

---

## ✅ Audio Folders Created

Folders have been created at:
- `ProgrammingProject/Audio/Music/`
- `ProgrammingProject/Audio/SFX/`

---

## 🎵 Required Audio Files

### Music Files (MP3) - Place in `Audio/Music/`:
1. background_music.mp3
2. combat_music.mp3
3. ruined_city.mp3
4. industrial.mp3
5. hollow_woods.mp3
6. cemetery.mp3
7. old_mill.mp3
8. canal.mp3
9. pump_station.mp3

### Sound Effects (WAV) - Place in `Audio/SFX/`:
1. combat_attack.wav
2. combat_hit.wav
3. combat_miss.wav
4. zombie_death.wav
5. loot_pickup.wav
6. environmental_hazard.wav
7. level_up.wav
8. skill_unlock.wav
9. save_game.wav

### Free Resources:
- **Music**: incompetech.com, freepd.com
- **SFX**: freesound.org, zapsplat.com

---

## 🧪 Testing After Implementation

1. Build project (should compile with 0 errors)
2. Enter combat
3. Attack zombie
4. Take damage
5. Kill zombie
6. **Expected**: See "RECENT ACTIONS:" with last 5 actions listed

---

## Summary

**Manual Steps Required**:
1. Edit GameplayEngine.h (2 changes)
2. Edit GameplayEngine.cpp (4 changes)
3. Add audio files to folders

**Total**: 6 code edits + audio files
