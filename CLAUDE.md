# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

"OUTBREAK" is a C++ console-based zombie apocalypse survival game built for Visual Studio 2022. It's a complete text-based RPG with exploration, turn-based combat, inventory management, skill progression, crafting system, and a branching narrative across 10 chapters with 52 collectible lore items.

**Story**: Player character Tony Redgrave (default name), a former water treatment engineer, must survive the Necrosis Virus outbreak and reach The Sanctuary while uncovering the truth about how it all began.

**Starting Stats**: 125 HP, 8 base damage, 2 skill points, starting items: Bandage (x2), Canned Food (x2)

## Building and Running

This project uses Visual Studio 2022 (v143 toolset) and targets Windows x64 only.

### Build Commands

**Using Visual Studio**:
1. Open `ProgrammingProject.sln`
2. Select configuration: `Debug|x64` or `Release|x64`
3. Build → Build Solution (Ctrl+Shift+B)
4. Run: `x64/Debug/ProgrammingProject.exe`

**Using MSBuild (from Developer Command Prompt)**:
```bash
# Debug build
msbuild ProgrammingProject.sln /p:Configuration=Debug /p:Platform=x64

# Release build
msbuild ProgrammingProject.sln /p:Configuration=Release /p:Platform=x64

# Clean and rebuild
msbuild ProgrammingProject.sln /p:Configuration=Debug /p:Platform=x64 /t:Clean,Build
```

**Output**: `x64/Debug/ProgrammingProject.exe` or `x64/Release/ProgrammingProject.exe`

### Dependencies
- Windows SDK 10.0
- Windows Multimedia API (`winmm.lib`) for background music and sound effects
- Optional: `background_music.mp3` in working directory for audio
- Optional: Sound effect `.wav` files (see `SOUND_EFFECTS_GUIDE.md`)

## Architecture Overview

### Triple Singleton Pattern
Three singleton classes manage different aspects of the game:
- **GameEngine** - Game state, locations, story progression, clue journal, save/load, main game loop
- **AudioEngine** - Background music and sound effects (menu, combat, loot, hazards)
- **GameplayEngine** - Exploration, turn-based combat, loot interaction, hazard checks

**Critical Access Pattern**:
```cpp
GameEngine* engine = GameEngine::getInstance();
AudioEngine* audio = AudioEngine::getInstance();
GameplayEngine* gameplay = GameplayEngine::getInstance();

// Always destroy in reverse order before exit:
AudioEngine::destroyInstance();
GameplayEngine::destroyInstance();
GameEngine::destroyInstance();
```

### Main Game Loop Flow
The game now features a **continuous loop** that returns to the title screen after each play session:

```cpp
// main.cpp structure
bool gameRunning = true;
while (gameRunning) {
    TitleScreen titleScreen("OUTBREAK");
    int choice = titleScreen.run();

    switch (choice) {
        case 0: engine->handleNewGame(); break;
        case 1: engine->handleLoadGame(); break;
        case 2: gameRunning = false; break;  // Exit
    }
}
```

When player quits via the in-game menu, `exploring` flag becomes false, which returns control to main.cpp, which then displays the title screen again.

### Entity Hierarchy
```
Entity (base)
├── Player - levels, stats, inventory, skill tree
├── Zombie (base)
│   ├── CommonInfected - Fast, low HP
│   ├── Boomer - Explodes on death (acid damage)
│   ├── Spitter - Ranged acid attacks
│   ├── Smoker - Tongue grab ensnare
│   └── Tank - Boss-level, high HP/damage
├── Location - Map nodes with connections, items, hazards, chapter info
└── Item - Categories, effects, consumables
```

Each zombie implements: `specialAttack()`, `chooseAttack()`, `getAccuracy()`, `applyStatusEffect()`, `onDeath()`, `canUseSpecialAbility()`, `useSpecialAbility()`, `getSpecialAbilityChance()`

### Custom Data Structures

**All implemented from scratch (no STL for core game logic)**:
- `SinglyLinkedList<T>` - Player inventory, skill children, location connections, crafting recipes
- `DoublyLinkedList<T>` - ClueJournal (52 lore items with bidirectional traversal)
- `Stack<T>` - Adapter around DoublyLinkedList
- `Queue<T>` - Adapter around SinglyLinkedList (zombie wave combat)
- `HashTable<Key, Value>` - Skill tree O(1) lookup

All use **sentinel node pattern** (NIL) with custom iterators (`SinglyLinkedNodeIterator`, `DoublyLinkedNodeIterator`).

**Iterator const-correctness**: Both `SinglyLinkedList` and `DoublyLinkedList` now have const and non-const versions of `begin()` and `end()`.

### Key Systems

#### Exploration System (GameplayEngine)
- Directional movement: `go left/right/up/down`
- 50% zombie encounter chance per movement
- Environmental hazard damage on movement
- Exploration progress: 0-12 steps to unlock travel
- Commands: `status`, `inventory`, `clues`, `skills`, `craft`, `rest`, `save`, `travel`, `menu`

#### Combat System (GameplayEngine)
- Turn-based with zombie waves (Queue-based FIFO)
- 3-6 zombies per wave (increased from original)
- Spawn rates: 40% Boomer, 25% Spitter, 20% Smoker, 15% Tank
- Combat actions: Attack (80% hit), Dodge (40% success), Use Item, Flee (50% success)
- XP gain: 10 XP per zombie killed
- **Special abilities**: Zombies now trigger special abilities during combat with `canUseSpecialAbility()` and `getSpecialAbilityChance()`
- **Death effects**: Zombies trigger `onDeath()` (e.g., Boomer explosion)
- Sound effects integrated for attacks, hits, misses, deaths

#### Skill Tree System (**FULLY FUNCTIONAL**)
- Access via `skills` command during exploration (NOT in menu)
- Player earns skill points from leveling (3 SP per level)
- Tree structure with parent-child prerequisites
- Skill lookup via HashTable using **skill IDs** (not names)
- 5 skill categories: Combat, Survival, Medical, Scavenging, Crafting
- Skills provide cumulative bonuses applied via `Player::applySkillBonuses()`
- **Critical**: Uses `fSkillID` for lookups, not `fSkillName`
- **Synchronization**: Player `fSkillPoints` syncs with `SkillTree::fAvailablePoints` via `setSkillPoints()`

**Skill Bonus Application**:
```cpp
// Player.cpp - Applied after unlock/upgrade and on load
void Player::applySkillBonuses() {
    // Reset to base stats
    fDamage = fBaseDamage;
    fMaxHealth = fBaseMaxHealth;

    // Apply skill tree bonuses
    int damageBonus = fSkillTree.getTotalDamageBonus();
    int healthBonus = fSkillTree.getTotalHealthBonus();

    fDamage += damageBonus;
    fMaxHealth += healthBonus;
    // ... other bonuses
}
```

#### Crafting System (**NEW**)
- Access via `craft` command during exploration
- `CraftingSystem` class manages recipes (SinglyLinkedList)
- Recipes require materials (items with Category::MATERIAL)
- Materials: Cloth, Herbs, Water, Antibiotics, Scrap Metal
- Craftable items: Health Potion, Advanced Bandage, Reinforced Armor
- Checks inventory for materials and space before crafting
- Integration: Called from `GameEngine::runExplorationLoop()` at line 1239-1299

#### Save/Load System (**FULLY RESTORED**)
- 10 save slots: `save_slot_1.txt` through `save_slot_10.txt`
- **Complete state persistence**:
  - Player stats (name, level, health, damage, XP, skill points)
  - Current location ID and visited status
  - Current chapter number
  - Exploration progress (stepsToNewLocation, movementSteps)
  - Equipped weapon
  - Entire inventory (pipe-delimited serialization)
  - **Picked up loot IDs** (prevents respawn)
  - **Collected clue IDs** (prevents duplication)

**Save File Format** (line-by-line):
```
Player Name
Player ID
Level
Damage
Health
Max Health
Location ID
Location Visited (0/1)
Chapter Number
Exploration Progress
Movement Steps
Equipped Weapon
XP
Skill Points
Inventory Size
[Items: ID|Name|Category|Desc|Qty|Space|Consumable|Usable|HP|Hunger|Infection|Damage]
Picked Up Loot Count
[Loot IDs]
Collected Clue Count
[Clue IDs]
```

**Load Flow**:
1. `GameEngine::loadGame()` reads save file
2. Creates Player, restores stats
3. Sets `currentLocation` via `getLocationByID()`
4. Stores exploration progress in temporary variables
5. Restores picked up loot and collected clues
6. `handleLoadGame()` initializes GameplayEngine FIRST
7. Then restores exploration progress to GameplayEngine
8. Displays chapter info and runs exploration loop

#### Loot & Clue System
**Loot tracking** prevents respawn:
```cpp
struct Loot {
    std::string lootID;  // Format: "loot_location_###"
    Item item;
    Direction location;
    bool isPickedUp;
};

// Global tracking
std::vector<std::string> pickedUpLootIDs;

// On pickup
addPickedUpLootID(loot.lootID);

// On populate
for (auto& loot : currentLocationLoot) {
    if (isLootPickedUp(loot.lootID)) {
        loot.isPickedUp = true;
    }
}
```

**Clue tracking** prevents duplication:
```cpp
// ClueJournal tracks collected IDs
std::vector<int> getCollectedClueIDs() const;
void setCollectedClueIDs(const std::vector<int>& clueIDs);

// On populate
for (auto& clue : currentLocationClues) {
    if (journal->isClueCollected(clue.clueID)) {
        clue.collected = true;
    }
}
```

#### Inventory System
- `SinglyLinkedList<Item>` with space tracking (default 20 slots)
- Categories: WEAPON, MEDICAL, FOOD, MATERIAL, TOOL, KEY_ITEM, CONSUMABLE
- **Weapon equipping**: Type `e` in inventory → applies damage boost to player
- **Item usage**: Type `u` in inventory → consumes item, applies effects
- **Dropping**: Type `d` in inventory → removes item, frees space
- Enhanced display with category indicators and effects

#### UI/Menu System
- `TitleScreen` - Main menu (DO NOT MODIFY per user request)
- `NavigationMenu` - Arrow key navigation with Windows API
- Enhanced HUD with **ASCII progress bars**:
  - Health bar: `[=====     ] 50/100`
  - Hunger bar: `[==========] 100/100`
  - XP bar: `[===       ] 30/150`
  - Exploration bar: `[======    ] 50% (6/12)`
- Commands always visible at bottom with separators

### File Organization

**Core Game Files**:
- `main.cpp` - Entry point with game loop that returns to title screen
- `GameEngine.h/cpp` - Singleton managing state, saves, skill tree UI, exploration loop
- `GameplayEngine.h/cpp` - Singleton managing exploration, combat, loot (~1000 lines)
- `AudioEngine.h/cpp` - Singleton managing music and sound effects
- `Crafting.h/cpp` - Crafting system with recipes (**NEW**)

**Entity System**:
- `Entity.h/cpp` - Base class
- `Player.h/cpp` - Player with skill bonuses, base stats tracking
- `Item.h/cpp` - Item system with effects
- `Zombie.h/cpp` - Base zombie with special abilities
- `CommonInfected.h/cpp`, `Boomer.h/cpp`, `Spitter.h/cpp`, `Smoker.h/cpp`, `Tank.h/cpp`

**Progression**:
- `SkillNode.h/cpp` - Skills with ID, bonuses, tree relationships
- `SkillTree.h/cpp` - Manages skill economy, lookups via HashTable

**Story**:
- `ClueJournal.h/cpp` - 52 lore items with DoublyLinkedList
- `Location.h/cpp` - Graph nodes with connections, 10 chapters

**Data Structures** (header-only templates):
- `SinglyLinkedNode.h`, `SinglyLinkedList.h`, `SinglyLinkedNodeIterator.h`
- `DoublyLinkedNode.h`, `DoublyLinkedList.h`, `DoublyLinkedNodeIterator.h`
- `Stack.h`, `Queue.h`, `HashTable.h`

## Critical Implementation Details

### Memory Management
- Manual new/delete (no smart pointers)
- Player heap-allocated in `handleNewGame()`/`handleLoadGame()`, deleted after exploration loop
- **New game reset**: `handleNewGame()` now deletes and recreates `journal`, clears `allLocations`
- Singletons destroyed via `destroyInstance()` in reverse order
- Zombie pointers in Queue managed by combat system

### Platform-Specific Code
- **Windows-only**: Uses `<Windows.h>`, `<mmsystem.h>`, MCI
- **NOMINMAX macro**: Always define before Windows.h to prevent `std::numeric_limits::max()` conflicts
```cpp
#define NOMINMAX
#include <Windows.h>
#include <limits>  // Safe to use
```

### Iterator Usage
```cpp
// CORRECT - const reference
for (auto it = inventory.begin(); it != inventory.end(); ++it) {
    const Item& item = *it;
}

// CORRECT - non-const reference (if modifying)
for (auto it = inventory.begin(); it != inventory.end(); ++it) {
    Item& item = *it;
    item.setQuantity(5);
}
```

### Skill Tree Critical Points

**1. Skill ID vs Skill Name**:
```cpp
// CORRECT - Use skill ID for lookups
SkillNode* skill = skillTree.getSkill(skillID);  // skillID = "combat_melee_1"

// WRONG - Don't use skill name
SkillNode* skill = skillTree.getSkill("Melee Combat");  // FAILS
```

**2. Skill Points Synchronization**:
```cpp
// Player.h - setSkillPoints syncs with SkillTree
void setSkillPoints(int points) {
    fSkillPoints = points;
    int currentTreePoints = fSkillTree.getAvailablePoints();
    int difference = points - currentTreePoints;
    if (difference > 0) {
        fSkillTree.addSkillPoints(difference);
    } else if (difference < 0) {
        fSkillTree.spendSkillPoints(-difference);
    }
}
```

**3. Bonus Application**:
```cpp
// ALWAYS call after unlock/upgrade/load
player->applySkillBonuses();
```

### Save/Load Critical Points

**1. Location Source**:
```cpp
// GameEngine::saveGame - Get location from GameplayEngine (authoritative)
GameplayEngine* gameplay = GameplayEngine::getInstance();
Location* activeLocation = gameplay->getCurrentLocation();
file << activeLocation->getID() << "\n";
```

**2. Load Order**:
```cpp
// GameEngine::handleLoadGame - Initialize GameplayEngine BEFORE displaying chapter
GameplayEngine* gameplay = GameplayEngine::getInstance();
gameplay->initialize(player, getCurrentLocation(), getJournal());
gameplay->setExplorationProgress(savedExplorationProgress);
// NOW display chapter info
```

**3. State Reset on New Game**:
```cpp
// GameEngine::handleNewGame - Clean slate
delete journal;
journal = new ClueJournal();
for (Location* loc : allLocations) delete loc;
allLocations.clear();
initializeAllLocations();
initializeAllLoreItems();
```

### Combat System Details

**Zombie Special Abilities**:
```cpp
// In combat loop
if (currentZombie->canUseSpecialAbility() &&
    rand() % 100 < currentZombie->getSpecialAbilityChance()) {
    std::string msg = currentZombie->useSpecialAbility(currentPlayer);
    int specialDmg = (int)(zombieAttackDmg * 1.5f);
    currentPlayer->takeDamage(specialDmg);
}
```

**Death Effects**:
```cpp
// After zombie defeated
currentZombie->onDeath(currentPlayer);  // Boomer explosion, etc.
```

### Crafting System Integration

**Recipe Structure**:
```cpp
CraftingRecipe* recipe = new CraftingRecipe("recipe_health_potion",
    "Health Potion", "Restores 50 HP",
    Item(...), 5);  // 5 seconds craft time
recipe->addMaterial("mat_herbs", 2);
recipe->addMaterial("mat_water", 1);
```

**Crafting Flow**:
```cpp
CraftingSystem craftingSystem;
craftingSystem.initializeRecipes();
if (craftingSystem.canCraft(recipe, player)) {
    if (craftingSystem.craftItem(recipe, player)) {
        // Success - materials consumed, item added
    }
}
```

## Common Patterns

### Adding New Skills
```cpp
// 1. Create with ID (for HashTable lookup)
SkillNode* skill = new SkillNode(
    "combat_melee_1",  // ID for lookup
    "Melee Combat",     // Display name
    "Increases melee damage. +5 damage",
    SkillNode::SkillType::COMBAT,
    1,  // Cost in SP
    3   // Max level
);

// 2. Set bonuses
skill->setDamageBonus(5);

// 3. Add to tree
skillTree.addRootSkill(skill);
fSkillLookup.insert("combat_melee_1", skill);  // Use ID!

// 4. For child skills
parentSkill->addChild(childSkill);
fSkillLookup.insert("combat_melee_2", childSkill);
```

### Adding New Crafting Recipes
```cpp
// In CraftingSystem::initializeRecipes()
Item resultItem("health_potion", "Health Potion", Item::Category::CONSUMABLE,
    "Restores 50 HP", 1, 2, true, true, 50, 0, 0, 0);

CraftingRecipe* recipe = new CraftingRecipe(
    "recipe_health_potion",
    "Health Potion",
    "Craft a healing potion",
    resultItem,
    5  // craft time
);

recipe->addMaterial("mat_herbs", 2);
recipe->addMaterial("mat_water", 1);
addRecipe(recipe);
```

### Adding Location Loot (with tracking)
```cpp
// GameplayEngine::populateLocationLoot()
if (locID == "loc_new_area") {
    Item sword("loot_new_sword", "Sword", Item::Category::WEAPON,
        "Sharp blade. 30 damage", 1, 8, false, true, 0, 0, 0, 30);
    currentLocationLoot.push_back(
        Loot("loot_new_area_001", sword, Direction::LEFT)
    );
}

// Auto-filtering at end of function
for (auto& loot : currentLocationLoot) {
    for (const auto& pickedUpID : pickedUpLootIDs) {
        if (loot.lootID == pickedUpID) {
            loot.isPickedUp = true;
            break;
        }
    }
}
```

## Common Issues and Solutions

### PDB File Lock Errors
```bash
# Delete debug folder and rebuild
rm -rf "x64/Debug"
msbuild ProgrammingProject.sln /p:Configuration=Debug /p:Platform=x64 /t:Rebuild
```

### Skill Tree Not Working
**Symptoms**: Points show garbage value, can't unlock skills

**Solution**: Ensure synchronization:
```cpp
// When loading
player->setSkillPoints(skillPoints);  // This syncs with SkillTree
player->applySkillBonuses();          // This applies stat bonuses
```

### Loot/Clues Respawning
**Symptoms**: Items appear again after reload

**Solution**: Check tracking:
```cpp
// Save includes picked up loot
file << gameplay->getPickedUpLootCount() << "\n";
for (const auto& lootID : gameplay->getPickedUpLootIDs()) {
    file << lootID << "\n";
}

// Load restores tracking
gameplay->setPickedUpLootIDs(pickedUpLootIDs);
journal->setCollectedClueIDs(collectedClueIDs);
```

### Location Not Saving
**Symptoms**: Load game starts at wrong location

**Solution**: Get location from GameplayEngine (not GameEngine):
```cpp
// GameEngine::saveGame
GameplayEngine* gameplay = GameplayEngine::getInstance();
Location* activeLocation = gameplay->getCurrentLocation();
```

### Menu Quit Doesn't Return to Title
**Symptoms**: Game exits instead of showing title screen

**Solution**: Use game loop in main.cpp:
```cpp
bool gameRunning = true;
while (gameRunning) {
    // Title screen, play session, repeat
}
```

## World Structure

**10 Chapters/Locations**:
1. Ruined City - Tutorial, Common Infected
2. Industrial District - Boomers/Spitters, TOXIC_FOG (12 HP)
3. Hollow Woods - Smokers, psychological horror
4. Old Mill - Tank boss, COLLAPSED_FLOOR (18 HP)
5. Overgrown Cemetery - Gate Key to Sanctuary
6. Polluted Canal - CONTAMINATED_WATER (15 HP)
7. Pump Station - Tony's workplace, TOXIC_FOG (14 HP)
8. Suburban Wasteland - Human tragedy
9. Abandoned Hospital - Patient Zero boss
10. The Sanctuary - Final destination

**52 Lore Items** distributed across all locations provide story context, combat hints, crafting recipes.

## Requirements Fulfillment

| Requirement | Status | Implementation |
|------------|--------|----------------|
| **Inheritance** | ✅ | Entity → Player, Zombie (5 types), Location, Item |
| **Polymorphism** | ✅ | Virtual methods in zombies (special abilities, death effects) |
| **Hash Table** | ✅ | SkillTree O(1) lookup via fSkillLookup |
| **Singly Linked List** | ✅ | Inventory, connections, recipes, skill children |
| **Doubly Linked List** | ✅ | ClueJournal (52 items + bidirectional) |
| **Stack** | ✅ | Available (DoublyLinkedList adapter) |
| **Queue** | ✅ | Zombie waves (FIFO combat) |
| **Tree** | ✅ | SkillTree with parent-child prerequisites |
| **Iterator** | ✅ | All structures with const/non-const versions |
| **Singleton** | ✅ | GameEngine + AudioEngine + GameplayEngine |

All 9/9 requirements fulfilled.
