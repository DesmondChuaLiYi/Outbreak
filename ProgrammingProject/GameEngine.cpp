#define NOMINMAX
#include "GameEngine.h"
#include "GameplayEngine.h"
#include "Crafting.h"
#include "CommonInfected.h"
#include "Boomer.h"
#include "Spitter.h"
#include "Smoker.h"
#include "Tank.h"
#include <iostream>
#include <limits>
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>
#include <cstdio>

// Initialize static member
GameEngine* GameEngine::instance = nullptr;

// Constructor
GameEngine::GameEngine() : currentPlayer(nullptr), currentLocation(nullptr),
	journal(nullptr), currentChapter(1), gameRunning(true),
	savedExplorationProgress(0), savedMovementSteps(0) {
	journal = new ClueJournal();
}

// Destructor
GameEngine::~GameEngine() {
	// Clean up locations
	for (Location* loc : allLocations) {
		delete loc;
	}
	allLocations.clear();

	if (journal != nullptr) {
		delete journal;
		journal = nullptr;
	}
}

// Get singleton instance
GameEngine* GameEngine::getInstance() {
	if (instance == nullptr) {
		instance = new GameEngine();
	}
	return instance;
}

// Initialize the game engine
bool GameEngine::initialize() {
	//std::cout << "[ENGINE] Initializing GameEngine...\n";
	setupConsoleWindow();
	initializeAllLocations();
	initializeAllLoreItems();
	//std::cout << "[ENGINE] Game world initialized with " << allLocations.size() << " locations.\n";
	//std::cout << "[ENGINE] Lore system initialized with " << journal->getTotalCluesInGame() << " clues.\n";
	return true;
}

// Setup console window
void GameEngine::setupConsoleWindow() {
#ifdef _WIN32
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	HWND consoleWindow = GetConsoleWindow();

	if (consoleWindow == NULL || handle == INVALID_HANDLE_VALUE)
		return;

	// Set buffer size
	COORD bufferSize = { (SHORT)CONSOLE_WIDTH, (SHORT)CONSOLE_HEIGHT };
	SetConsoleScreenBufferSize(handle, bufferSize);

	// Set window size (matching buffer)
	SMALL_RECT windowSize = { 0, 0, (SHORT)(CONSOLE_WIDTH - 1), (SHORT)(CONSOLE_HEIGHT - 1) };
	SetConsoleWindowInfo(handle, TRUE, &windowSize);

	std::cout << "[ENGINE] Console initialized: "
		<< CONSOLE_WIDTH << "x" << CONSOLE_HEIGHT << "\n";
#endif
}

// Game state management
void GameEngine::setPlayer(Player* player) {
	currentPlayer = player;
}

Player* GameEngine::getPlayer() {
	return currentPlayer;
}

void GameEngine::setCurrentLocation(Location* location) {
	currentLocation = location;
	if (location != nullptr) {
		currentChapter = location->getChapterNumber();
	}
}

Location* GameEngine::getCurrentLocation() {
	return currentLocation;
}

ClueJournal* GameEngine::getJournal() {
	return journal;
}

// Location management
Location* GameEngine::getLocationByID(const std::string& locationID) {
	for (Location* loc : allLocations) {
		if (loc->getID() == locationID) {
			return loc;
		}
	}
	return nullptr;
}

std::vector<Location*>& GameEngine::getAllLocations() {
	return allLocations;
}

// Story progression
void GameEngine::displayChapterIntro() {
	if (currentLocation != nullptr && !currentLocation->isVisited()) {
		currentLocation->displayChapterIntro();
		currentLocation->markVisited();

		std::cout << "\n\nPress ENTER to continue...";
		std::cin.get();
	}
}

int GameEngine::getCurrentChapter() const {
	return currentChapter;
}

void GameEngine::advanceChapter() {
	currentChapter++;
}

// Game loop control
bool GameEngine::isGameRunning() const {
	return gameRunning;
}

void GameEngine::setGameRunning(bool running) {
	gameRunning = running;
}

// Traversal
bool GameEngine::travelToLocation(const std::string& locationID) {
	// Check if current location is connected to target
	if (currentLocation == nullptr) {
		std::cout << "[ERROR] No current location set.\n";
		return false;
	}

	if (!currentLocation->isConnectedTo(locationID)) {
		std::cout << "[ERROR] Cannot travel to " << locationID << " - not connected.\n";
		return false;
	}

	Location* targetLocation = getLocationByID(locationID);
	if (targetLocation == nullptr) {
		std::cout << "[ERROR] Location " << locationID << " not found.\n";
		return false;
	}

	// Travel to the location
	setCurrentLocation(targetLocation);
	std::cout << "\n[TRAVEL] Traveling to " << targetLocation->getName() << "...\n\n";

	// Display chapter intro if first visit
	displayChapterIntro();

	return true;
}

void GameEngine::displayTravelOptions() {
	if (currentLocation == nullptr) {
		std::cout << "[ERROR] No current location.\n";
		return;
	}

	std::cout << "\n========================================\n";
	std::cout << "Available Locations to Travel:\n";
	std::cout << "========================================\n";

	SinglyLinkedList<std::string>& connections = currentLocation->getConnections();
	int option = 1;

	for (auto it = connections.begin(); it != connections.end(); ++it) {
		Location* loc = getLocationByID(*it);
		if (loc != nullptr) {
			std::string visitedMark = loc->isVisited() ? "[VISITED]" : "[NEW]";
			std::cout << option++ << ". " << loc->getName()
				<< " " << visitedMark;
			if (loc->getChapterNumber() > 0) {
				std::cout << " (Chapter " << loc->getChapterNumber() << ")";
			}
			std::cout << "\n";
		}
	}

	std::cout << "========================================\n";
}

// Helper method to create a location with chapter info
Location* GameEngine::createLocation(const std::string& id, const std::string& name,
	Location::LocationType type, const std::string& description,
	const std::string& atmosphere, Location::Hazard hazard, int hazardDamage,
	int chapterNumber, const std::string& chapterTitle, const std::string& chapterStory) {

	Location* loc = new Location(id, name, type, description, atmosphere, hazard, hazardDamage, "", false);
	loc->setChapterInfo(chapterNumber, chapterTitle, chapterStory);
	return loc;
}

// Initialize all locations with story chapters
void GameEngine::initializeAllLocations() {
	// Chapter 1: Ruined City
	Location* ruinedCity = createLocation(
		"loc_ruined_city",
		"Ruined City",
		Location::LocationType::CITY,
		"Collapsed high-rises, burned-out cars, shattered storefronts. Graffiti warns: 'THE WATER KILLED US'.",
		"Silent except for distant groans. Thin haze of smoke carries the metallic scent of blood.",
		Location::Hazard::NONE,
		0,
		1,
		"Ruined City: The Awakening",
		"The city is silent except for the distant groans of the infected. Streets are littered with burnt vehicles, toppled lampposts, and scattered debris. A thin haze of smoke hangs in the air, carrying the faint metallic scent of blood. Graffiti covers walls in frantic handwriting: 'THE WATER KILLED US'. Bodies of the recently deceased are everywhere - some twisted in fear, some in quiet repose, others barely recognizable as human.\n\nYou are Tony Redgrave, a former water treatment engineer. You know the truth behind the outbreak, and every step you take is haunted by the knowledge that human negligence caused this catastrophe. Your goal is The Sanctuary, a fortified settlement with clean water, food, and security - but every mile is a test of survival, morality, and sanity."
	);
	// Add zombies to Ruined City
	ruinedCity->addZombie(new CommonInfected("zombie_city_1", "Common Infected"));
	ruinedCity->addZombie(new CommonInfected("zombie_city_2", "Common Infected"));
	allLocations.push_back(ruinedCity);

	// Chapter 2: Industrial District
	Location* industrial = createLocation(
		"loc_industrial",
		"Industrial District",
		Location::LocationType::INDUSTRIAL,
		"Rusted factories belching toxic smoke. Green sludge bubbles from cracked containers.",
		"Smells of chemicals and rust. Occasional boomer patrols the area.",
		Location::Hazard::TOXIC_FOG,
		12,  // Increased from 5
		2,
		"Industrial District: Toxic Horror",
		"The Industrial District smells of chemicals and rust. Green sludge bubbles from cracked containers, and grotesque Boomers - obese infected that explode with acid upon death - patrol the area. You must navigate carefully, as detonations attract nearby infected, creating dangerous chain reactions.\n\nScavenging an abandoned warehouse, you find evidence of corporate negligence: 'Dispose of medical waste in river. Cost-effective.' Among the crates, you collect an axe, gas mask, and scrap metal. A Spitter appears in the shadows, hurling acidic bile. The Industrial District is where environmental hazards and zombie types combine, teaching the interplay of combat, stealth, and resource management."
	);
	industrial->addZombie(new Boomer("zombie_ind_1", "Boomer"));
	industrial->addZombie(new Spitter("zombie_ind_2", "Spitter"));
	allLocations.push_back(industrial);

	// Chapter 3: Hollow Woods
	Location* hollowWoods = createLocation(
		"loc_hollow_woods",
		"Hollow Woods",
		Location::LocationType::FOREST,
		"Dense trees with mist drifting between them. Animal carcasses hang from twisted branches.",
		"Isolation and fear. Nature perished long before humanity fell.",
		Location::Hazard::DARKNESS,
		2,
		3,
		"Hollow Woods: Isolation and Fear",
		"The forest looms as you enter the Hollow Woods. Mist drifts between dense trees, and animal carcasses hang from twisted branches - a grim reminder that nature perished long before humanity fell. Here, Common Infected stalk in waves, but Smoker-type infected lurk - tall, emaciated figures with elongated tongues capable of ensnaring survivors from afar.\n\nA hidden campsite provides a hunting rifle, ammo, and a journal reading: 'The animals died first. Then the people. Now it's just waiting. Sometimes I swear I hear them whispering my name.' The journal hints at the psychological toll of isolation. A lone survivor corpse presents a choice: loot the supplies or bury the body, restoring a sliver of your mental state."
	);
	hollowWoods->addZombie(new CommonInfected("zombie_woods_1", "Common Infected"));
	hollowWoods->addZombie(new Smoker("zombie_woods_2", "Smoker"));
	allLocations.push_back(hollowWoods);

	// Chapter 4: Old Mill
	Location* oldMill = createLocation(
		"loc_old_mill",
		"Old Mill",
		Location::LocationType::INDUSTRIAL,
		"Creaking floors and rotting wood. Smells of mildew and blood.",
		"Monument to human despair. Tank infected dominates the upper floors.",
		Location::Hazard::COLLAPSED_FLOOR,
		18,  // Increased from 10
		4,
		"Old Mill: The Butcher Awaits",
		"The Old Mill stands as a monument to human despair. Floors creak underfoot, and rotting wood smells of mildew and blood. A Tank - massive and unstoppable - dominates the upper floors, wielding a cleaver-like weapon. This is your first boss encounter.\n\nThe Mill contains a hidden lore item: The Butcher's Family Photo and Note, written in trembling ink: 'I couldn't save them.' The note forces you to confront human tragedy firsthand. Rescue attempts or mercy kills are possible but carry consequences. Defeating the Tank grants rare medkits, antibiotics, and a splint - a tangible reward for overcoming both fear and moral distress."
	);
	oldMill->addZombie(new Tank("zombie_mill_boss", "The Butcher"));
	allLocations.push_back(oldMill);

	// Chapter 5: Overgrown Cemetery
	Location* cemetery = createLocation(
		"loc_cemetery",
		"Overgrown Cemetery",
		Location::LocationType::SANCTUARY,
		"Toppled tombstones. Fog hangs thick. Sound of clawing from beneath the soil.",
		"Death is not peace. Fresh graves disturbed.",
		Location::Hazard::NONE,
		0,
		5,
		"Overgrown Cemetery: Death Is Not Peace",
		"Fog hangs thick among toppled tombstones. The sound of clawing from beneath the soil heralds the ambush of Common Infected, rising from freshly disturbed graves. Your senses are on high alert.\n\nThe Gravedigger's Diary reads: 'We buried them too soon. They came back.' The diary expands the lore, showing how desperate survivors attempted containment, only to fail. You find a shotgun, shells, gasoline, and cloth - but the true treasure is the Gate Key to the Sanctuary, hidden in an unmarked grave. A symbol of hope amidst despair."
	);
	cemetery->addZombie(new CommonInfected("zombie_cem_1", "Risen Corpse"));
	cemetery->addZombie(new CommonInfected("zombie_cem_2", "Risen Corpse"));
	allLocations.push_back(cemetery);

	// Chapter 6: Polluted Canal
	Location* canal = createLocation(
		"loc_canal",
		"Polluted Canal",
		Location::LocationType::INDUSTRIAL,
		"Industrial runoff colors the stagnant canal neon green. Walking through it risks infection.",
		"Toxic waters eat away at flesh and stamina.",
		Location::Hazard::CONTAMINATED_WATER,
		15,  // Increased from 8
		6,
		"Polluted Canal: Toxic Waters",
		"Industrial runoff colors the stagnant canal neon green. Walking through it risks infection, as toxins eat away at flesh and stamina. Bizarrely mutated Boomers wade through the sludge, and Spitters lurk along the edges, spraying acidic bile with deadly precision.\n\nYou salvage Water Purification Tablets and a gas mask filter. You find the Government Containment Report: 'Containment failed. Virus waterborne. All quarantine efforts collapsed.' The report adds political and systemic dimensions to the narrative - human negligence at a scale too great to ignore."
	);
	canal->addZombie(new Boomer("zombie_canal_1", "Toxic Boomer"));
	canal->addZombie(new Spitter("zombie_canal_2", "Canal Spitter"));
	allLocations.push_back(canal);

	// Chapter 7: Pump Station
	Location* pumpStation = createLocation(
		"loc_pump_station",
		"Pump Station",
		Location::LocationType::INDUSTRIAL,
		"Your former workplace. Corpses of colleagues in hazmat suits litter the floors.",
		"The truth revealed. This is where it all began.",
		Location::Hazard::TOXIC_FOG,
		14,  // Increased from 6
		7,
		"Pump Station: The Truth Revealed",
		"At the Pump Station, you face the truth: this was your workplace. Corpses of colleagues in hazmat suits litter the floors. Your own workstation note reads: 'I should have stopped them.' Here, a moral choice presents itself: activate a failsafe to purify water for future survivors, or escape to preserve personal survival.\n\nThe area contains medkits, antibiotics, and ammo, but every second is contested by Common Infected and the occasional Smoker, forcing strategic prioritization of threats. The weight of your past decisions bears down on you."
	);
	pumpStation->addZombie(new CommonInfected("zombie_pump_1", "Infected Worker"));
	pumpStation->addZombie(new Smoker("zombie_pump_2", "Smoker"));
	allLocations.push_back(pumpStation);

	// Chapter 8: Suburban Wasteland
	Location* suburban = createLocation(
		"loc_suburban",
		"Suburban Wasteland",
		Location::LocationType::SUBURBAN,
		"Families' lives interrupted mid-day: toys scattered, minivans crashed into garages.",
		"Human tragedy frozen in time.",
		Location::Hazard::NONE,
		0,
		8,
		"Suburban Wasteland: Human Tragedy",
		"The Suburban Wasteland shows families' lives interrupted mid-day: toys scattered, minivans crashed into garages. Zombies, primarily Common Infected, are present in large numbers. Some families are partially infected - you face harrowing moral decisions: fight or flee, loot or leave provisions.\n\nIn the Great Fields beyond, open grasslands reveal the scale of human collapse. Hordes of infected, including Boomers and Spitters, roam the fields, forcing strategic combat choices. The Farmer's Suicide Note reads: 'There's nothing left to grow. We tried. I failed.'"
	);
	suburban->addZombie(new CommonInfected("zombie_sub_1", "Suburban Infected"));
	suburban->addZombie(new CommonInfected("zombie_sub_2", "Suburban Infected"));
	suburban->addZombie(new Boomer("zombie_sub_3", "Bloated Infected"));
	allLocations.push_back(suburban);

	// Chapter 9: Abandoned Hospital
	Location* hospital = createLocation(
		"loc_hospital",
		"Abandoned Hospital",
		Location::LocationType::HOSPITAL,
		"Multi-story nightmare. Flickering lights illuminate biohazard signs and abandoned gurneys.",
		"Medical horror. Origins of catastrophe revealed.",
		Location::Hazard::DARKNESS,
		3,
		9,
		"Abandoned Hospital: Origins of Catastrophe",
		"The hospital is a multi-story nightmare. Flickering lights illuminate biohazard signs and abandoned gurneys. Here, you encounter Patient Zero - a unique boss infected, surrounded by staff who are themselves Common Infected. Defeating Patient Zero yields antibiotics, medkits, and the Doctor's Audio Recording: 'Patient Zero was exposed at the water plant. We failed everyone.'\n\nIn the nearby Quarantine Zone, military failure is evident. Armed infected soldiers, now Common Infected, roam abandoned checkpoints. The Military Orders provide chilling insight: 'Shoot on sight. No exceptions.' The full scope of humanity's collapse is laid bare."
	);
	hospital->addZombie(new Tank("zombie_hosp_boss", "Patient Zero"));
	hospital->addZombie(new CommonInfected("zombie_hosp_1", "Infected Doctor"));
	hospital->addZombie(new CommonInfected("zombie_hosp_2", "Infected Nurse"));
	allLocations.push_back(hospital);

	// Chapter 10: The Sanctuary
	Location* sanctuary = createLocation(
		"loc_sanctuary",
		"The Sanctuary",
		Location::LocationType::SANCTUARY,
		"A walled settlement with gardens, generators, and survivors. Entry requires the Gate Key.",
		"Fragile hope. The end of your journey, or a new beginning.",
		Location::Hazard::NONE,
		0,
		10,
		"The Sanctuary: Fragile Hope",
		"You approach The Sanctuary, a walled settlement with gardens, generators, and survivors. Entry requires the Gate Key you found in the cemetery. Here, the truth is laid bare: corporate greed and government negligence caused the outbreak.\n\nYou face the ultimate moral choice: stay and help rebuild, leave to wander the wasteland, sabotage the water system, or broadcast the truth to the wider survivor community. Your journey has brought you here, but the question remains: what kind of survivor will you be?"
	);
	allLocations.push_back(sanctuary);

	// Create connections (graph structure)
	// Chapter progression path
	ruinedCity->addConnection("loc_industrial");
	industrial->addConnection("loc_ruined_city");
	industrial->addConnection("loc_hollow_woods");
	hollowWoods->addConnection("loc_industrial");
	hollowWoods->addConnection("loc_old_mill");
	oldMill->addConnection("loc_hollow_woods");
	oldMill->addConnection("loc_cemetery");
	cemetery->addConnection("loc_old_mill");
	cemetery->addConnection("loc_canal");
	canal->addConnection("loc_cemetery");
	canal->addConnection("loc_pump_station");
	pumpStation->addConnection("loc_canal");
	pumpStation->addConnection("loc_suburban");
	suburban->addConnection("loc_pump_station");
	suburban->addConnection("loc_hospital");
	hospital->addConnection("loc_suburban");
	hospital->addConnection("loc_sanctuary");
	sanctuary->addConnection("loc_hospital");

	// Set starting location
	setCurrentLocation(ruinedCity);
}

// Initialize all 52 lore items
void GameEngine::initializeAllLoreItems() {
	// Add all 52 lore items to the journal
	// Format: Clue(id, name, content, location, effect)

	// Ruined City clues (1-5)
	journal->addClue(Clue(1, "City Engineer's Journal", "Day 1: They knew the water was contaminated. They did nothing. If anyone reads this, know I tried. We failed.", "Ruined City", "Story insight: origin of outbreak, moral weight"));
	journal->addClue(Clue(2, "Graffiti: THE WATER KILLED US", "Spray-painted on a crumbling wall, faded but readable. Fear and warning for anyone who enters.", "Ruined City", "Story hint, sets tone"));
	journal->addClue(Clue(3, "Apartment Scrawled Note", "We hid in the apartment. They came anyway. I hope whoever finds this survives longer than we did.", "Ruined City", "Mental state +5 (empathy)"));
	journal->addClue(Clue(4, "Grocery Store Ledger", "Lists spoiled and missing food items, dated Day 2 of the outbreak.", "Ruined City", "Loot hint, story detail"));
	journal->addClue(Clue(5, "Crowbar Engraving", "Etched letters on crowbar: 'For survival.'", "Ruined City", "Minor morale boost"));

	// Industrial District clues (6-10)
	journal->addClue(Clue(6, "Industrial Memo", "Dispose of medical waste in river. Cost-effective. Safety is secondary to profits.", "Industrial District", "Story insight: corporate negligence"));
	journal->addClue(Clue(7, "Toxic Drum Label", "Warning: Corrosive. Handle with care.", "Industrial District", "Environmental storytelling"));
	journal->addClue(Clue(8, "Warehouse Blueprint", "Sketches of warehouse layout, showing supply routes.", "Industrial District", "Crafting hint (location of loot)"));
	journal->addClue(Clue(9, "Smoker Observation Log", "Notes: Acidic excretions may incapacitate prey. Avoid direct contact.", "Industrial District", "Combat hint: Smoker behavior"));
	journal->addClue(Clue(10, "Boomer Corpse Notes", "Scribbled on wall near exploded Boomer: 'Fatty won't stop anyone alive.'", "Industrial District", "Story flavor, horror immersion"));

	// Hollow Woods clues (11-15)
	journal->addClue(Clue(11, "Hiker's Journal", "The animals died first. Then the people. Now it's just waiting. Sometimes I swear I hear it whispering my name.", "Hollow Woods", "Mental state impact, story depth"));
	journal->addClue(Clue(12, "Hidden Campsite Ledger", "Lists survival items and recipes for basic crafting.", "Hollow Woods", "Crafting hint"));
	journal->addClue(Clue(13, "Tree Carving", "SARA + JIM = SAFE scratched into bark.", "Hollow Woods", "Emotional story hint"));
	journal->addClue(Clue(14, "Survivor Corpse Letter", "If you find this, take my food, but don't forget us.", "Hollow Woods", "Moral choice: bury or loot"));
	journal->addClue(Clue(15, "Hunting Rifle Notes", "Clean and oil regularly, it might save your life.", "Hollow Woods", "Weapon tip"));

	// Old Mill clues (16-19)
	journal->addClue(Clue(16, "The Butcher's Family Photo", "I couldn't save them. Photo shows family smiling, a cruel reminder.", "Old Mill", "Story insight, emotional weight"));
	journal->addClue(Clue(17, "Mill Floor Graffiti", "Death waits above. Don't trust the creak.", "Old Mill", "Atmospheric storytelling"));
	journal->addClue(Clue(18, "Cleaver Scrap Notes", "Mechanics notes, how to sharpen or maintain the cleaver.", "Old Mill", "Weapon upgrade hint"));
	journal->addClue(Clue(19, "Mercy-Kill Choice Prompt", "The survivor begs. Will you end their suffering?", "Old Mill", "Moral choice, mental state"));

	// Overgrown Cemetery clues (20-22)
	journal->addClue(Clue(20, "Gravedigger's Diary", "We buried them too soon. They came back. I can't sleep. They claw at the graves.", "Overgrown Cemetery", "Story insight, mental state -10"));
	journal->addClue(Clue(21, "Tombstone Messages", "Names and dates of victims, some scratched with warnings.", "Overgrown Cemetery", "Environmental storytelling"));
	journal->addClue(Clue(22, "Cemetery Gate Key", "Hidden in an unmarked grave.", "Overgrown Cemetery", "Unlocks Sanctuary, story progression"));

	// Polluted Canal clues (23-25)
	journal->addClue(Clue(23, "Gas Mask Filter Notes", "Replace every 5 hours. Sludge will kill you if you ignore it.", "Polluted Canal", "Survival hint"));
	journal->addClue(Clue(24, "Contaminated Water Journal", "Tasted it once… fever came two hours later. Don't make the same mistake.", "Polluted Canal", "Infection awareness"));
	journal->addClue(Clue(25, "Government Containment Report", "Containment failed. Virus waterborne. All quarantine efforts collapsed.", "Polluted Canal", "Story insight, expands world lore"));

	// Pump Station clues (26-29)
	journal->addClue(Clue(26, "Tony's Workstation Note", "I should have stopped them. I signed off on the protocols, ignored the signs. Everything we knew was ignored.", "Pump Station", "Story insight, moral weight"));
	journal->addClue(Clue(27, "Hazmat Suit Remnants", "Torn suits with initials.", "Pump Station", "Environmental storytelling, possible loot"));
	journal->addClue(Clue(28, "Emergency Cache List", "Antibiotics x3, Medkits x2, Ammo x20.", "Pump Station", "Loot hint"));
	journal->addClue(Clue(29, "Survivor Message on Control Panel", "If anyone finds this… finish what we started.", "Pump Station", "Story motivation"));

	// Suburban Wasteland clues (30-35)
	journal->addClue(Clue(30, "Parent's Diary", "We waited for help. It never came. Our children were scared. We failed them.", "Suburban Wasteland", "Emotional weight, mental state impact"));
	journal->addClue(Clue(31, "Toy Soldier Collection", "Scattered on the floor.", "Suburban Wasteland", "Environmental storytelling, nostalgia"));
	journal->addClue(Clue(32, "Locked Basement Note", "Supplies hidden here. Keep quiet.", "Suburban Wasteland", "Loot hint"));
	journal->addClue(Clue(33, "Farmer's Suicide Note", "There's nothing left to grow. The land is dead. I am too.", "Great Fields", "Emotional story, moral weight"));
	journal->addClue(Clue(34, "Barn Ledger", "Lists remaining food and ammo caches.", "Great Fields", "Crafting/loot hint"));
	journal->addClue(Clue(35, "Horde Warning Chalk Marks", "X marks drawn on barn walls: 'DO NOT ENTER – THEY ARE EVERYWHERE.'", "Great Fields", "Combat hint"));

	// Railway Station clues (36-38)
	journal->addClue(Clue(36, "Evacuation Manifest", "10,000 boarded. 3 trains left. Most never reached safety.", "Railway Station", "Story insight"));
	journal->addClue(Clue(37, "Luggage Tags", "Names of people who fled.", "Railway Station", "Environmental storytelling"));
	journal->addClue(Clue(38, "Platform Graffiti", "All hope gone. Don't trust anyone.", "Railway Station", "Tone-setting, immersion"));

	// River Crossing clues (39-41)
	journal->addClue(Clue(39, "Riverside Cabin Note", "Raft materials stored here. Be careful of patrols.", "River Crossing", "Crafting hint"));
	journal->addClue(Clue(40, "Rope & Wood Logs", "Bundled and ready for building raft.", "River Crossing", "Crafting materials"));
	journal->addClue(Clue(41, "Flare Gun Note", "Use only if absolutely necessary. Might attract infected.", "River Crossing", "Survival choice"));

	// Abandoned Hospital clues (42-45)
	journal->addClue(Clue(42, "Patient Zero Observation Report", "Highly infectious. Avoid contact. Use any means necessary.", "Abandoned Hospital", "Story insight, boss context"));
	journal->addClue(Clue(43, "Doctor's Audio Recording", "Patient Zero was exposed at the water plant. We failed everyone.", "Abandoned Hospital", "Story insight"));
	journal->addClue(Clue(44, "Surgery Kit Label", "One chance to remove infection. Handle carefully.", "Abandoned Hospital", "Gameplay item, rare survival tool"));
	journal->addClue(Clue(45, "Hospital Gurney Notes", "Staff tried to secure patients. Supplies ran out.", "Abandoned Hospital", "Story flavor"));

	// Quarantine Zone clues (46-48)
	journal->addClue(Clue(46, "Military Orders", "Shoot on sight. No exceptions. Humanity is a lost cause.", "Quarantine Zone", "Story insight, world-building"));
	journal->addClue(Clue(47, "Abandoned MREs Label", "Dates show supplies expired days before collapse.", "Quarantine Zone", "Survival hint"));
	journal->addClue(Clue(48, "Assault Rifle Manual", "Loaded and maintained. Might be the difference between life and death.", "Quarantine Zone", "Weapon tip"));

	// Sanctuary Approach clues (49-52)
	journal->addClue(Clue(49, "Sanctuary Gate Map", "Shows layout and key access points. Handle with caution.", "Sanctuary Approach", "Unlock / story hint"));
	journal->addClue(Clue(50, "Survivor Graffiti Messages", "We made it this far. Don't let them take it from us.", "Sanctuary Approach", "Tone-setting, hope"));
	journal->addClue(Clue(51, "Boomer Corpse Wall Etching", "Warning: bigger ones will explode. Stay back.", "Industrial & Great Fields", "Combat hint"));
	journal->addClue(Clue(52, "Smoker Snare Notes", "Tongue can pull a survivor into danger. Watch your flanks.", "Hollow Woods & Pump Station", "Combat hint"));
}

// Cleanup and destroy singleton
void GameEngine::destroyInstance() {
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}

// ============================================================================
// HELPER UI FUNCTIONS
// ============================================================================

void clearConsole() {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}

void centerText(const std::string& text) {
	int padding = (GameEngine::CONSOLE_WIDTH - text.length()) / 2;
	if (padding < 0) padding = 0;
	std::cout << std::string(padding, ' ') << text;
}

void drawSeparator() {
	std::cout << std::string(GameEngine::CONSOLE_WIDTH, '=') << "\n";
}

// ============================================================================
// CHARACTER CREATION
// ============================================================================

Player* GameEngine::createCharacter() {
	clearConsole();
	std::cout << "\n";
	drawSeparator();
	centerText("CHARACTER CREATION\n");
	drawSeparator();
	std::cout << "\n";

	centerText("Welcome to OUTBREAK, Survivor...\n");
	std::cout << "\n\n";

	std::string playerName;
	centerText("Enter your character name: ");
	std::cin.ignore(10000, '\n');  // FIX: Clear input buffer
	std::getline(std::cin, playerName);

	// Validate name
	while (playerName.empty() || playerName.length() > 20) {
		if (playerName.empty()) {
			centerText("Name cannot be empty! Try again: ");
		} else {
			centerText("Name too long (max 20 chars)! Try again: ");
		}
		std::getline(std::cin, playerName);
	}

	clearConsole();
	std::cout << "\n\n";
	drawSeparator();
	std::cout << "\n";
	centerText("Creating character: " + playerName + "\n");
	std::cout << "\n";
	drawSeparator();
	std::cout << "\n\n";

	Player* newPlayer = new Player("player_001", playerName, 1, 10, 100, 100);

	// Add starting items
	Item bandage("item_001", "Bandage", Item::Category::MEDICAL,
		"Basic medical supplies. Restores 15 HP.", 3, 1, true, true, 15, 0, 0, 0);
	Item bread("item_002", "Bread", Item::Category::FOOD,
		"Nutritious bread. Restores 20 hunger.", 2, 1, true, true, 0, 20, 0, 0);
	Item crowbar("item_003", "Crowbar", Item::Category::WEAPON,
		"Sturdy iron crowbar. Basic melee weapon.", 1, 3, false, true, 0, 0, 0, 10);

	newPlayer->addItem(bandage);
	newPlayer->addItem(bread);
	newPlayer->addItem(crowbar);

	centerText("[SUCCESS] Character created!\n");
	std::cout << "\n";
	centerText("Welcome, " + newPlayer->getName() + "!\n");
	std::cout << "\n";

	std::this_thread::sleep_for(std::chrono::milliseconds(1500));

	return newPlayer;
}

// ============================================================================
// GAME MENU
// ============================================================================

int GameEngine::displayGameMenu() {
	clearConsole();
	std::cout << "\n";
	drawSeparator();
	centerText("OUTBREAK - GAME MENU\n");
	drawSeparator();
	std::cout << "\n\n";

	centerText("1. View Current Location\n");
	std::cout << "\n";
	centerText("2. Travel to Another Location\n");
	std::cout << "\n";
	centerText("3. View Player Status\n");
	std::cout << "\n";
	centerText("4. View Collected Clues\n");
	std::cout << "\n";
	centerText("5. Save Game\n");
	std::cout << "\n";
	centerText("6. Quit Game\n");
	std::cout << "\n\n";
	drawSeparator();
	std::cout << "\n";

	centerText("Enter choice: ");
	int choice;
	std::cin >> choice;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	return choice;
}

// ============================================================================
// GAME LOOP
// ============================================================================

void GameEngine::gameLoop(Player* player) {
	setPlayer(player);

	// Display starting chapter
	displayChapterIntro();

	bool running = true;
	while (running && isGameRunning()) {
		int choice = displayGameMenu();
		handleMenuChoice(choice, player, running);
	}
}

void GameEngine::handleMenuChoice(int choice, Player* player, bool& running) {
	switch (choice) {
	case 1: { // View Current Location
		clearConsole();
		Location* currentLoc = getCurrentLocation();
		if (currentLoc != nullptr) {
			currentLoc->displayLocationInfo();
			std::cout << "\n";
			currentLoc->displayConnections();
		}
		std::cout << "\n\nPress ENTER to continue...";
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		break;
	}

	case 2: { // Travel to Another Location
		clearConsole();
		Location* currentLoc = getCurrentLocation();
		if (currentLoc != nullptr) {
			std::cout << "\n";
			drawSeparator();
			centerText("TRAVEL MENU\n");
			drawSeparator();
			std::cout << "\n";

			// Display current location
			centerText("Current Location: " + currentLoc->getName() + "\n");
			std::cout << "\n";

			// Display travel options
			displayTravelOptions();

			// Get player choice
			std::cout << "\nEnter location number to travel (0 to cancel): ";
			int travelChoice;
			std::cin >> travelChoice;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			if (travelChoice > 0) {
				// Get the location ID from connections
				SinglyLinkedList<std::string>& connections = currentLoc->getConnections();
				int count = 1;
				for (auto it = connections.begin(); it != connections.end(); ++it) {
					if (count == travelChoice) {
						travelToLocation(*it);
						break;
					}
					count++;
				}
			}
		}
		break;
	}

	case 3: { // View Player Status
		clearConsole();
		std::cout << "\n";
		drawSeparator();
		centerText("PLAYER STATUS\n");
		drawSeparator();
		std::cout << "\n\n";
		player->displayInformation();
		std::cout << "\n\n";
		centerText("Inventory: " + std::to_string(player->getInventorySize()) + " items\n");
		std::cout << "\n\nPress ENTER to continue...";
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		break;
	}

	case 4: { // View Collected Clues
		clearConsole();
		ClueJournal* journalRef = getJournal();
		if (journalRef != nullptr) {
			journalRef->displayProgress();
			std::cout << "\n";
			if (journalRef->getTotalCollected() > 0) {
				journalRef->displayCluesSummary();
			}
		}
		std::cout << "\n\nPress ENTER to continue...";
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		break;
	}

	case 5: { // Save Game
		clearConsole();
		std::cout << "\n";
		drawSeparator();
		centerText("SAVE GAME\n");
		drawSeparator();
		std::cout << "\n";

		displaySaveSlots();

		int slot = selectSaveSlot(false);
		if (slot > 0) {
			if (saveGame(player, slot)) {
				std::cout << "\n";
				centerText("[SUCCESS] Game saved to slot " + std::to_string(slot) + "!\n");
			}
			else {
				std::cout << "\n";
				centerText("[ERROR] Failed to save game.\n");
			}
		}

		std::cout << "\n\nPress ENTER to continue...";
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		break;
	}

	case 6: { // Quit
		clearConsole();
		std::cout << "\n\n";
		centerText("Thank you for playing OUTBREAK.\n");
		std::cout << "\n";
		centerText("Stay safe, survivor...\n\n");
		running = false;
		break;
	}

	default:
		std::cout << "\nInvalid choice. Press ENTER to continue...";
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		break;
	}
}

// ============================================================================
// SAVE/LOAD SYSTEM
// ============================================================================

void GameEngine::displaySaveSlots() {
	std::cout << "\n";
	centerText("Available Save Slots:\n");
	std::cout << "\n";

	for (int i = 1; i <= 10; i++) {
		std::string filename = "save_slot_" + std::to_string(i) + ".txt";
		std::ifstream file(filename);

		std::string slotInfo = std::to_string(i) + ". ";
		if (file.is_open()) {
			std::string playerName, playerID, locationID;
			int level;
			
			// Read save file data
			std::getline(file, playerName);  // Line 1: Player name
			std::getline(file, playerID);    // Line 2: Player ID
			file >> level;                   // Line 3: Level
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Skip damage
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Skip health
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Skip max health
			std::getline(file, locationID);  // Line 7: Location ID
			
			// Get location name from ID
			std::string locationName = "Unknown";
			Location* loc = getLocationByID(locationID);
			if (loc != nullptr) {
				locationName = loc->getName();
			}
			
			slotInfo += "[OCCUPIED] " + playerName + " Lvl" + std::to_string(level) + " - Map: " + locationName;
			file.close();
		}
		else {
			slotInfo += "[EMPTY]";
		}

		centerText(slotInfo + "\n");
	}

	std::cout << "\n";
}

int GameEngine::selectSaveSlot(bool isLoading) {
	std::cout << "\n";
	if (isLoading) {
		centerText("Enter slot number to load (0 to cancel, -1 to delete): ");
	}
	else {
		centerText("Enter slot number to save (0 to cancel): ");
	}

	int slot;
	std::cin >> slot;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	if (slot < -1 || slot > 10) {
		centerText("[ERROR] Invalid slot number.\n");
		return 0;
	}

	if (slot == -1 && isLoading) {
		// Delete slot option
		centerText("Enter slot number to delete (0 to cancel): ");
		int deleteSlot;
		std::cin >> deleteSlot;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		if (deleteSlot > 0 && deleteSlot <= 10) {
			if (deleteSaveSlot(deleteSlot)) {
				centerText("[SUCCESS] Save slot " + std::to_string(deleteSlot) + " deleted.\n");
			}
			else {
				centerText("[ERROR] Failed to delete save slot.\n");
			}
		}
		return 0;
	}

	return slot;
}

bool GameEngine::saveGame(Player* player, int slotNumber) {
	if (player == nullptr || slotNumber < 1 || slotNumber > 10) {
		return false;
	}

	std::string filename = "save_slot_" + std::to_string(slotNumber) + ".txt";
	std::ofstream file(filename);

	if (!file.is_open()) {
		return false;
	}

	try {
		// Save player data
		file << player->getName() << "\n";
		file << player->getID() << "\n";
		file << player->getLevel() << "\n";
		file << player->getDamage() << "\n";
		file << player->getHealth() << "\n";
		file << player->getMaxHealth() << "\n";

		// Save current location (get from GameplayEngine as it's the authoritative source)
		GameplayEngine* gameplay = GameplayEngine::getInstance();
		Location* activeLocation = gameplay->getCurrentLocation();
		if (activeLocation != nullptr) {
			file << activeLocation->getID() << "\n";
			file << (activeLocation->isVisited() ? 1 : 0) << "\n";
		}
		else if (currentLocation != nullptr) {
			file << currentLocation->getID() << "\n";
			file << (currentLocation->isVisited() ? 1 : 0) << "\n";
		}
		else {
			file << "loc_ruined_city\n";
			file << "0\n";
		}

		// Save current chapter
		file << currentChapter << "\n";

		// Save exploration progress
		file << gameplay->getExplorationProgress() << "\n";
		file << gameplay->getMovementSteps() << "\n";

		// Save player equipped weapon
		file << player->getEquippedWeapon() << "\n";

		// Save player XP and skill points
		file << player->getExperience() << "\n";
		file << player->getSkillPoints() << "\n";

		// Save inventory count
		file << player->getInventorySize() << "\n";

		// Save each item in inventory
		SinglyLinkedList<Item>& inventory = player->getInventory();
		for (auto it = inventory.begin(); it != inventory.end(); ++it) {
			const Item& item = *it;
			file << item.getID() << "|"
				<< item.getName() << "|"
				<< static_cast<int>(item.getCategory()) << "|"
				<< item.getDescription() << "|"
				<< item.getQuantity() << "|"
				<< item.getInventorySpace() << "|"
				<< item.isConsumable() << "|"
				<< item.isUsable() << "|"
				<< item.getHealthRestore() << "|"
				<< item.getHungerRestore() << "|"
				<< item.getInfectionCure() << "|"
				<< item.getDamageBoost() << "\n";
		}

		// Save picked up loot status
		file << gameplay->getPickedUpLootCount() << "\n";
		for (const auto& lootID : gameplay->getPickedUpLootIDs()) {
			file << lootID << "\n";
		}

		// Save collected clues
		file << journal->getCollectedClueCount() << "\n";
		for (int clueID : journal->getCollectedClueIDs()) {
			file << clueID << "\n";
		}

		file.close();
		return true;
	}
	catch (...) {
		file.close();
		return false;
	}
}

Player* GameEngine::loadGame(int slotNumber) {
	if (slotNumber < 1 || slotNumber > 10) {
		return nullptr;
	}

	std::string filename = "save_slot_" + std::to_string(slotNumber) + ".txt";
	std::ifstream file(filename);

	if (!file.is_open()) {
		return nullptr;
	}

	try {
		std::string playerName, playerID, locationID, equippedWeapon;
		int level, damage, health, maxHealth, chapter, inventorySize, locationVisited;
		int explorationProgress, movementSteps, xp, skillPoints;

		std::getline(file, playerName);
		std::getline(file, playerID);
		file >> level >> damage >> health >> maxHealth;
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::getline(file, locationID);
		file >> locationVisited;
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		file >> chapter;
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		file >> explorationProgress >> movementSteps;
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::getline(file, equippedWeapon);
		file >> xp >> skillPoints;
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		file >> inventorySize;
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Create player
		Player* player = new Player(playerID, playerName, level, damage, health, maxHealth);

		// Restore player state
		player->setEquippedWeapon(equippedWeapon);
		player->setXP(xp);
		player->setSkillPoints(skillPoints);

		// Load inventory
		for (int i = 0; i < inventorySize; i++) {
			std::string line;
			std::getline(file, line);

			// Parse item data (pipe-delimited)
			size_t pos = 0;
			std::vector<std::string> tokens;
			while ((pos = line.find('|')) != std::string::npos) {
				tokens.push_back(line.substr(0, pos));
				line.erase(0, pos + 1);
			}
			tokens.push_back(line); // Last token

			if (tokens.size() == 12) {
				Item item(
					tokens[0], // ID
					tokens[1], // Name
					static_cast<Item::Category>(std::stoi(tokens[2])),
					tokens[3], // Description
					std::stoi(tokens[4]), // Quantity
					std::stoi(tokens[5]), // InventorySpace
					std::stoi(tokens[6]) != 0, // isConsumable
					std::stoi(tokens[7]) != 0, // isUsable
					std::stoi(tokens[8]), // healthRestore
					std::stoi(tokens[9]), // hungerRestore
					std::stoi(tokens[10]), // infectionCure
					std::stoi(tokens[11])  // damageBoost
				);
				player->addItem(item);
			}
		}

		// Load picked up loot IDs
		int pickedUpLootCount;
		file >> pickedUpLootCount;
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		std::vector<std::string> pickedUpLootIDs;
		for (int i = 0; i < pickedUpLootCount; i++) {
			std::string lootID;
			std::getline(file, lootID);
			pickedUpLootIDs.push_back(lootID);
		}

		// Load collected clue IDs
		int collectedClueCount;
		file >> collectedClueCount;
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		std::vector<int> collectedClueIDs;
		for (int i = 0; i < collectedClueCount; i++) {
			int clueID;
			file >> clueID;
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			collectedClueIDs.push_back(clueID);
		}

		file.close();

		// Set location
		Location* loc = getLocationByID(locationID);
		if (loc != nullptr) {
			setCurrentLocation(loc);
			// Restore visited status
			if (locationVisited) {
				loc->markVisited();
			}
		}

		// Set chapter
		currentChapter = chapter;

		// Store exploration progress and loot/clue state to restore after GameplayEngine is initialized
		savedExplorationProgress = explorationProgress;
		savedMovementSteps = movementSteps;

		// Restore picked up loot IDs to GameplayEngine (will be applied in handleLoadGame)
		GameplayEngine* gameplay = GameplayEngine::getInstance();
		gameplay->setPickedUpLootIDs(pickedUpLootIDs);

		// Restore collected clues to journal
		journal->setCollectedClueIDs(collectedClueIDs);

		return player;
	}
	catch (...) {
		file.close();
		return nullptr;
	}
}

bool GameEngine::deleteSaveSlot(int slotNumber) {
	if (slotNumber < 1 || slotNumber > 10) {
		return false;
	}

	std::string filename = "save_slot_" + std::to_string(slotNumber) + ".txt";

	// Check if file exists
	std::ifstream checkFile(filename);
	if (!checkFile.is_open()) {
		return false; // File doesn't exist
	}
	checkFile.close();

	// Delete the file
	if (std::remove(filename.c_str()) == 0) {
		return true;
	}

	return false;
}

// ============================================================================
// MAIN MENU HANDLERS
// ============================================================================

void GameEngine::handleNewGame() {
	// RESET GameEngine state for new game
	currentChapter = 1;
	currentLocation = nullptr;
	
	// Clear and reinitialize journal
	if (journal != nullptr) {
		delete journal;
		journal = nullptr;
	}
	journal = new ClueJournal();
	
	// Clear all locations
	for (Location* loc : allLocations) {
		delete loc;
	}
	allLocations.clear();
	
	// Reinitialize everything fresh
	initializeAllLocations();
	initializeAllLoreItems();
	
	system("cls");
	std::cout << "\n\n" << std::string(80, '=') << "\n";
	std::cout << "  NEW GAME\n";
	std::cout << std::string(80, '=') << "\n\n";

	std::string name;
	std::cout << "  Enter name (or press ENTER for 'Tony Redgrave'): ";
	std::getline(std::cin, name);
	
	if (name.empty()) name = "Tony Redgrave";

	Player* player = new Player("player_001", name, 1, 8, 125, 125);

	// Give starting skill points for unlocking first skill
	player->setSkillPoints(2);

	// Starting items
	player->addItem(Item("start_bandage", "Bandage", Item::Category::MEDICAL,
		"Restores 15 HP", 2, 3, true, true, 15, 0, 0, 0));
	player->addItem(Item("start_food", "Canned Food", Item::Category::FOOD,
		"Restores 30 hunger", 2, 4, true, true, 0, 30, 0, 0));

	system("cls");
	std::cout << "\n\n" << std::string(80, '=') << "\n";
	std::cout << "  WELCOME, " << name << "\n";
	std::cout << std::string(80, '=') << "\n\n";
	std::cout << "  Three months after Day Zero...\n";
	std::cout << "  The Necrosis Virus has consumed the world.\n";
	std::cout << "  Your goal: Reach The Sanctuary.\n";
	std::cout << "  Your burden: Knowing how it all began.\n\n";
	std::cout << "  Press ENTER...";
	std::cin.get();

	displayChapterIntro();
	GameplayEngine* gameplay = GameplayEngine::getInstance();
	gameplay->initialize(player, getCurrentLocation(), getJournal());

	// Apply initial skill bonuses
	player->applySkillBonuses();

	runExplorationLoop(player);

	delete player;
}

void GameEngine::handleLoadGame() {
	system("cls");
	std::cout << "\n\n" << std::string(80, '=') << "\n";
	std::cout << "  LOAD GAME\n";
	std::cout << std::string(80, '=') << "\n\n";

	displaySaveSlots();
	int slot = selectSaveSlot(true);

	if (slot > 0) {
		Player* player = loadGame(slot);
		if (player != nullptr) {
			system("cls");
			std::cout << "\n\n" << std::string(80, '=') << "\n";
			std::cout << "  GAME LOADED\n";
			std::cout << std::string(80, '=') << "\n\n";
			std::cout << "  Welcome back, " << player->getName() << "!\n\n";
			std::cout << "  Press ENTER to continue...";
			std::cin.get();

			// Initialize GameplayEngine FIRST before displaying chapter
			GameplayEngine* gameplay = GameplayEngine::getInstance();
			gameplay->initialize(player, getCurrentLocation(), getJournal());

			// Restore exploration progress
			gameplay->setExplorationProgress(savedExplorationProgress);
			gameplay->setMovementSteps(savedMovementSteps);

			// Display current chapter and location info
			system("cls");
			if (currentLocation != nullptr) {
				// Update currentChapter from location
				currentChapter = currentLocation->getChapterNumber();
				
				std::cout << "\n" << std::string(80, '=') << "\n";
				std::cout << "  CHAPTER " << currentChapter << ": "
					<< currentLocation->getChapterTitle() << "\n";
				std::cout << std::string(80, '=') << "\n\n";
				std::cout << "  " << currentLocation->getDescription() << "\n\n";
				std::cout << "  Press ENTER to continue...";
				std::cin.get();
			}

			// Run the exploration loop
			runExplorationLoop(player);
			delete player;
		}
		else {
			system("cls");
			std::cout << "\n  [ERROR] Failed to load.\n";
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		}
	}
}

void GameEngine::runGame() {
	// This method is currently not used, but kept for potential future use
	// Main game loop is handled by gameLoop()
}

// ============================================================================
// EXPLORATION LOOP
// ============================================================================

void GameEngine::runExplorationLoop(Player* player) {
	GameplayEngine* gameplay = GameplayEngine::getInstance();
	bool exploring = true;

	while (exploring && player->getHealth() > 0) {
		gameplay->displayCurrentLocation();

		std::cout << "Command: ";
		std::string command;
		std::getline(std::cin, command);

		// Convert to lowercase
		for (char& c : command) c = tolower(c);

		if (command.find("go ") == 0) {
			std::string dir = command.substr(3);
			size_t start = dir.find_first_not_of(" \t");
			if (start != std::string::npos) {
				dir = dir.substr(start);
			}

			Direction direction = gameplay->stringToDirection(dir);
			if (direction != Direction::CENTER) {
				gameplay->moveInDirection(direction);
			}
			else {
				system("cls");
				std::cout << "\n  Invalid direction.\n  Press ENTER...";
				std::cin.get();
			}
		}
		else if (command == "status") {
			gameplay->displayPlayerStatus();
		}
		else if (command == "inventory") {
			gameplay->displayInventory();
		}
		else if (command == "clues") {
			gameplay->displayCollectedClues();
		}
		else if (command == "skills") {
			displaySkillTreeMenu(player);
		}
		else if (command == "craft") {
			system("cls");
			std::cout << "\n" << std::string(80, '=') << "\n";
			std::cout << "  CRAFTING MENU\n";
			std::cout << std::string(80, '=') << "\n\n";
			
			// Create a local crafting system
			CraftingSystem craftingSystem;
			craftingSystem.initializeRecipes();
			
			// Display available recipes
			std::cout << "  AVAILABLE RECIPES:\n\n";
			
			SinglyLinkedList<CraftingRecipe*>& recipes = craftingSystem.getRecipes();
			int recipeIndex = 1;
			std::vector<CraftingRecipe*> recipeList;
			
			for (auto it = recipes.begin(); it != recipes.end(); ++it) {
				CraftingRecipe* recipe = *it;
				recipeList.push_back(recipe);
				
				bool canCraft = craftingSystem.canCraft(recipe, player);
				std::string status = canCraft ? "[CRAFTABLE]" : "[NEED MATERIALS]";
				
				std::cout << "  [" << recipeIndex << "] " << recipe->recipeName << " " << status << "\n";
				std::cout << "      " << recipe->description << "\n";
				std::cout << "      Materials:\n";
				
				for (const auto& mat : recipe->materials) {
					std::cout << "        - " << mat.first << " (x" << mat.second << ")\n";
				}
				std::cout << "\n";
				recipeIndex++;
			}
			
			std::cout << "  [0] Back\n\n";
			std::cout << "  Enter recipe number to craft: ";
			
			int recipeChoice;
			std::cin >> recipeChoice;
			std::cin.ignore();
			
			if (recipeChoice > 0 && recipeChoice <= (int)recipeList.size()) {
				CraftingRecipe* selectedRecipe = recipeList[recipeChoice - 1];
				
				if (craftingSystem.canCraft(selectedRecipe, player)) {
					if (craftingSystem.craftItem(selectedRecipe, player)) {
						system("cls");
						std::cout << "\n  [SUCCESS] Crafted: " << selectedRecipe->recipeName << "\n";
						std::cout << "  [+] Added to inventory!\n";
						std::cout << "  Press ENTER...";
						std::cin.get();
					}
				} else {
					system("cls");
					std::cout << "\n  [ERROR] Cannot craft - missing materials or inventory full!\n";
					std::cout << "  Press ENTER...";
					std::cin.get();
				}
			}
		}
		else if (command == "travel") {
			if (gameplay->canTravelToNewLocation()) {
				gameplay->displayTravelOptions();
				
				int choice;
				std::cin >> choice;
				std::cin.ignore();

				if (choice > 0) {
					// Get connection at index
					SinglyLinkedList<std::string>& connections = gameplay->getCurrentLocation()->getConnections();
					int count = 1;
					for (auto it = connections.begin(); it != connections.end(); ++it) {
						if (count == choice) {
							gameplay->travelToLocation(*it);
							break;
						}
						count++;
					}
				}
			}
			else {
				system("cls");
				std::cout << "\n  You haven't explored enough yet.\n";
				std::cout << "  Explore more (15 steps needed).\n  Press ENTER...";
				std::cin.get();
			}
		}
		else if (command == "rest") {
			system("cls");
			std::cout << "\n  You rest...\n";
			int heal = 25;
			int newHP = player->getHealth() + heal;
			if (newHP > player->getMaxHealth()) {
				newHP = player->getMaxHealth();
			}
			player->setHealth(newHP);
			std::cout << "  Restored " << heal << " HP!\n";
			std::cout << "  HP: " << player->getHealth() << "/" << player->getMaxHealth() << "\n";
			std::cout << "\n  Press ENTER...";
			std::cin.get();
		}
		else if (command == "save") {
			system("cls");
			std::cout << "\n  SAVE GAME\n\n";
			displaySaveSlots();
			
			int slot = selectSaveSlot(false);
			if (slot > 0) {
				if (saveGame(player, slot)) {
					std::cout << "\n  [SUCCESS] Game saved to slot " << slot << "!\n";
				}
				else {
				 std::cout << "\n  [ERROR] Failed to save game.\n";
				}
			}
			std::cout << "\n  Press ENTER...";
			std::cin.get();
		}
		else if (command == "menu") {
			system("cls");
			std::cout << "\n  GAME MENU\n\n";
			std::cout << "  [1] Continue Exploring\n";
			std::cout << "  [2] Save Game\n";
			std::cout << "  [3] Quit to Main Menu\n\n";
			std::cout << "  Choice: ";

			int choice;
			std::cin >> choice;
			std::cin.ignore();

			if (choice == 1) {
				// Continue exploring
			}
			else if (choice == 2) {
				// Save game
				system("cls");
				std::cout << "\n  SAVE GAME\n\n";
				displaySaveSlots();

				int slot = selectSaveSlot(false);
				if (slot > 0) {
					if (saveGame(player, slot)) {
						std::cout << "\n  [SUCCESS] Game saved to slot " << slot << "!\n";
					}
					else {
						std::cout << "\n  [ERROR] Failed to save game.\n";
					}
				}
				std::cout << "\n  Press ENTER...";
				std::cin.get();
			}
			else if (choice == 3) {
				// Quit to main menu
				std::cout << "\n  Returning to main menu...\n";
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				
				// CRITICAL: Reset singleton instance for clean state
				GameplayEngine::destroyInstance();
				
				exploring = false;
			}
		}
		else {
			system("cls");
			std::cout << "\n  Unknown command.\n";
			std::cout << "  Available: go [left/right/up/down] | status | inventory | clues | rest | save";
			if (gameplay->canTravelToNewLocation()) {
				std::cout << " | travel";
			}
			std::cout << ", menu\n";
			std::cout << "  Press ENTER...";
			std::cin.get();
		}

		if (player->getHealth() <= 0) {
			system("cls");
			std::cout << "\n\n" << std::string(80, '=') << "\n";
			std::cout << "  GAME OVER\n";
			std::cout << std::string(80, '=') << "\n\n";
			std::cout << "  You succumbed to the outbreak...\n\n";
			std::cout << "  Press ENTER...";
			std::cin.get();
			break;
		}
	}
}

// Display skill tree menu and allow player to upgrade skills
void GameEngine::displaySkillTreeMenu(Player* player) {
	bool inSkillMenu = true;

	while (inSkillMenu) {
		system("cls");
		std::cout << "\n";
		std::cout << "================================================================\n";
		std::cout << "                        SKILL TREE                              \n";
		std::cout << "================================================================\n\n";

		// Display available skill points
		int availablePoints = player->getSkillPoints();
		std::cout << "  Available Skill Points: " << availablePoints << "\n\n";

		// Get skill tree reference
		SkillTree& skillTree = player->getSkillTree();

		// Display current bonuses
		std::cout << "  Current Total Bonuses:\n";
		int damageBonus = skillTree.getTotalDamageBonus();
		int healthBonus = skillTree.getTotalHealthBonus();
		int infectionRes = skillTree.getTotalInfectionResistance();
		float craftingBonus = skillTree.getTotalCraftingBonus();
		float scavengeBonus = skillTree.getTotalScavengeBonus();

		if (damageBonus > 0) std::cout << "    +" << damageBonus << " Damage\n";
		if (healthBonus > 0) std::cout << "    +" << healthBonus << " Max Health\n";
		if (infectionRes > 0) std::cout << "    +" << infectionRes << "% Infection Resistance\n";
		if (craftingBonus > 0) std::cout << "    -" << (int)(craftingBonus * 100) << "% Crafting Cost\n";
		if (scavengeBonus > 0) std::cout << "    +" << (int)(scavengeBonus * 100) << "% Loot Quality\n";

		std::cout << "\n";
		std::cout << "================================================================\n";
		std::cout << "                      AVAILABLE SKILLS                          \n";
		std::cout << "================================================================\n\n";

		// Display all skills
		SinglyLinkedList<SkillNode*>& rootSkills = skillTree.getRootSkills();
		int skillIndex = 1;
		std::vector<std::string> skillIDs;

		// Display root skills
		for (auto it = rootSkills.begin(); it != rootSkills.end(); ++it) {
			SkillNode* skill = *it;

			// Display skill info
			std::cout << "  [" << skillIndex << "] ";
			if (skill->isUnlocked()) {
				std::cout << "[UNLOCKED] ";
			} else {
				std::cout << "[LOCKED]   ";
			}
			std::cout << skill->getSkillName() << " (" << skill->typeToString() << ")\n";
			std::cout << "      " << skill->getSkillDescription() << "\n";
			std::cout << "    Cost: " << skill->getCost() << " SP | Level: " << skill->getLevel() << "/" << skill->getMaxLevel() << "\n";

			// Store the skill ID for lookup (not name)
			skillIDs.push_back(skill->getSkillID());
			skillIndex++;

			// Display children if root is unlocked
			if (skill->isUnlocked()) {
				SinglyLinkedList<SkillNode*>& children = skill->getChildren();
				for (auto childIt = children.begin(); childIt != children.end(); ++childIt) {
					SkillNode* childSkill = *childIt;

					std::cout << "[" << skillIndex << "] ";
					if (childSkill->isUnlocked()) {
						std::cout << "[UNLOCKED] ";
					} else {
						std::cout << "[LOCKED]   ";
					}
					std::cout << childSkill->getSkillName() << " (" << childSkill->typeToString() << ")\n";
					std::cout << "   " << childSkill->getSkillDescription() << "\n";
					std::cout << "     Cost: " << childSkill->getCost() << " SP | Level: " << childSkill->getLevel() << "/" << childSkill->getMaxLevel() << "\n";

					// Store the skill ID for lookup (not name)
					skillIDs.push_back(childSkill->getSkillID());
					skillIndex++;
				}
			}
			std::cout << "\n";
		}

		std::cout << "================================================================\n";
		std::cout << "  Enter skill number to unlock/upgrade (0 to go back): ";

		int choice;
		std::cin >> choice;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		if (choice == 0) {
			inSkillMenu = false;
		} else if (choice > 0 && choice <= (int)skillIDs.size()) {
			std::string selectedSkillID = skillIDs[choice - 1];
			SkillNode* selectedSkill = skillTree.getSkill(selectedSkillID);

			if (selectedSkill != nullptr) {
				// Check if skill is already unlocked
				if (selectedSkill->isUnlocked()) {
					// Try to level up
					if (selectedSkill->isMaxLevel()) {
						std::cout << "\n  This skill is already at max level!\n";
						std::cout << "  Press ENTER...";
						std::cin.get();
					} else {
						availablePoints = player->getSkillPoints(); // Refresh points
						if (availablePoints >= selectedSkill->getCost()) {
							if (skillTree.levelUpSkill(selectedSkillID)) {
								// Sync player skill points with SkillTree
								int newPoints = availablePoints - selectedSkill->getCost();
								player->setSkillPoints(newPoints);
								
								// Apply skill bonuses after leveling up
								player->applySkillBonuses();
								
								std::cout << "\n  [SUCCESS] Skill upgraded: " << selectedSkill->getSkillName() << "\n";
								std::cout << "  Level: " << selectedSkill->getLevel() << "/" << selectedSkill->getMaxLevel() << "\n";
								std::cout << "  Skill Points: " << player->getSkillPoints() << "\n";
								std::cout << "  Press ENTER...";
								std::cin.get();
							} else {
								std::cout << "\n  Failed to upgrade skill!\n";
								std::cout << "  Press ENTER...";
								std::cin.get();
							}
						} else {
							std::cout << "\n  Not enough skill points! Need " << selectedSkill->getCost() << ", have " << availablePoints << "\n";
							std::cout << "  Press ENTER...";
							std::cin.get();
						}
					}
				} else {
					// Try to unlock
					availablePoints = player->getSkillPoints(); // Refresh points
					if (availablePoints >= selectedSkill->getCost()) {
						// Check prerequisites
						SkillNode* parent = selectedSkill->getParent();
						if (parent != nullptr && !parent->isUnlocked()) {
							std::cout << "\n  You must unlock the parent skill first!\n";
							std::cout << "  Parent: " << parent->getSkillName() << "\n";
							std::cout << "  Press ENTER...";
							std::cin.get();
						} else {
							if (skillTree.unlockSkill(selectedSkillID)) {
								// Sync player skill points with SkillTree
								int newPoints = availablePoints - selectedSkill->getCost();
								player->setSkillPoints(newPoints);
								
								// Apply skill bonuses after unlocking
								player->applySkillBonuses();
								
								std::cout << "\n  [SUCCESS] Skill unlocked: " << selectedSkill->getSkillName() << "\n";
								std::cout << "  Skill Points: " << player->getSkillPoints() << "\n";
								std::cout << "  Press ENTER...";
								std::cin.get();
							} else {
								std::cout << "\n  Failed to unlock skill!\n";
								std::cout << "  Press ENTER...";
								std::cin.get();
							}
						}
					} else {
						std::cout << "\n  Not enough skill points! Need " << selectedSkill->getCost() << ", have " << availablePoints << "\n";
						std::cout << "  Press ENTER...";
						std::cin.get();
					}
				}
			} else {
				std::cout << "\n  Skill not found!\n";
				std::cout << "Press ENTER...";
				std::cin.get();
			}
		} else if (choice > 0 && choice <= (int)skillIDs.size()) {
			std::string selectedSkillID = skillIDs[choice - 1];
			SkillNode* selectedSkill = skillTree.getSkill(selectedSkillID);

			if (selectedSkill != nullptr) {
				// Check if skill is already unlocked
				if (selectedSkill->isUnlocked()) {
					// Try to level up
					if (selectedSkill->isMaxLevel()) {
						std::cout << "\n  This skill is already at max level!\n";
						std::cout << "  Press ENTER...";
						std::cin.get();
					} else {
						availablePoints = player->getSkillPoints(); // Refresh points
						if (availablePoints >= selectedSkill->getCost()) {
							if (skillTree.levelUpSkill(selectedSkillID)) {
								// Sync player skill points with SkillTree
								int newPoints = availablePoints - selectedSkill->getCost();
								player->setSkillPoints(newPoints);
								
								// Apply skill bonuses after leveling up
								player->applySkillBonuses();
								
								std::cout << "\n  [SUCCESS] Skill upgraded: " << selectedSkill->getSkillName() << "\n";
								std::cout << "  Level: " << selectedSkill->getLevel() << "/" << selectedSkill->getMaxLevel() << "\n";
								std::cout << "  Skill Points: " << player->getSkillPoints() << "\n";
								std::cout << "  Press ENTER...";
								std::cin.get();
							} else {
								std::cout << "\n  Failed to upgrade skill!\n";
								std::cout << "  Press ENTER...";
								std::cin.get();
							}
						} else {
							std::cout << "\n  Not enough skill points! Need " << selectedSkill->getCost() << ", have " << availablePoints << "\n";
							std::cout << "  Press ENTER...";
							std::cin.get();
						}
					}
				} else {
					// Try to unlock
					availablePoints = player->getSkillPoints(); // Refresh points
					if (availablePoints >= selectedSkill->getCost()) {
						// Check prerequisites
						SkillNode* parent = selectedSkill->getParent();
						if (parent != nullptr && !parent->isUnlocked()) {
							std::cout << "\n  You must unlock the parent skill first!\n";
							std::cout << "  Parent: " << parent->getSkillName() << "\n";
							std::cout << "  Press ENTER...";
							std::cin.get();
						} else {
							if (skillTree.unlockSkill(selectedSkillID)) {
								// Sync player skill points with SkillTree
								int newPoints = availablePoints - selectedSkill->getCost();
								player->setSkillPoints(newPoints);
								
								// Apply skill bonuses after unlocking
								player->applySkillBonuses();
								
								std::cout << "\n  [SUCCESS] Skill unlocked: " << selectedSkill->getSkillName() << "\n";
								std::cout << "  Skill Points: " << player->getSkillPoints() << "\n";
								std::cout << "  Press ENTER...";
								std::cin.get();
							} else {
								std::cout << "\n  Failed to unlock skill!\n";
								std::cout << "  Press ENTER...";
								std::cin.get();
							}
						}
					} else {
						std::cout << "\n  Not enough skill points! Need " << selectedSkill->getCost() << ", have " << availablePoints << "\n";
						std::cout << "  Press ENTER...";
						std::cin.get();
					}
				}
			} else {
				std::cout << "\n  Skill not found!\n";
				std::cout << "Press ENTER...";
				std::cin.get();
			}
		}
	}
}
