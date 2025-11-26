#include "TitleScreen.h"
#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <iomanip>
#include <thread>
#include <chrono>
#include <vector>

#ifdef _WIN32
#define CLEAR_SCREEN "cls"
#else
#define CLEAR_SCREEN "clear"
#endif

TitleScreen::TitleScreen(const std::string& title)
    : gameTitle(title), selectedIndex(0), lastRenderedIndex(-1), isRunning(false)
{
    menuOptions = { "New Game", "Load Game", "Exit Game" };
}

TitleScreen::~TitleScreen() {
}

void TitleScreen::clearScreen() {
    system(CLEAR_SCREEN);
}

void TitleScreen::centerText(const std::string& text) {
    int padding = (GameEngine::CONSOLE_WIDTH - (int)text.length()) / 2;
    if (padding < 0) padding = 0;
    std::cout << std::string(padding, ' ') << text;
}

void TitleScreen::leftAlignText(const std::string& text, int leftPadding) {
    std::cout << std::string(leftPadding, ' ') << text;
}

void TitleScreen::drawSeparator() {
    std::cout << std::string(GameEngine::CONSOLE_WIDTH, '=') << "\n";
}

void TitleScreen::drawThemedSeparator() {
    std::cout << std::string(GameEngine::CONSOLE_WIDTH, '-') << "\n";
}

void TitleScreen::drawTitle() {
	const std::vector<std::string> asciiArt = {
			"%%%%%%##****#####%###********************************************************************###*****%%%",
			"%%%%%%#******************************************************************************************##%",
	"%%##*****%#************+==-----------=+********************************************************####%",
	"%###**************+=------====------------=+*****************************************************###",
			"%*#************+====------=-------------------+*************************************************####",
		 "#************+===-------------------------------+***********************************************####",
			"#**********+===+=---------------------------------+*************************************************",
			"#*********=-----------------------------------------***********************************************#",
		"%********--------------------------------------------+***********************%%%%#****************#%",
	   "%#******----------------------------------------------+*********************#%%%%#****************##",
		"%******------------------------------------------------+********************%%%#******************##",
			"#*****=--------=========---------------=----==========--******************#%%%%%%#******************",
			"#*****--------------------------------------------------+***************#%%%%%%%%%%%***************#",
	 "%****=---------------------------------------------------***************%%%%%%%%%%%%#*********#%#***",
			"#****----------------------------------------------------+*************#%%%%%%%%%%%%%********#%%%***",
		  "#***+----------------------------------------------------+***%%#********%%%%%%%%%%%%%#******#%%%****",
			"#****---------=-----===+=====----------------------------+**%%%#******#%%%%%%%%%%%%%%***#%%%%%%%#**",
			"#*#%#==------=--------=====+===------===----==+----------*%%%%%%*******#%%%%%%%%%%%*%%%**%%%%%%%%%**",
			"##%#*+--------++++==-------==-===----------=----------=%%%%%%%%#*******%%#%%%%%%%%%*%%%##%%%%%%%%%#*",
			"#*****-------=----------------=------====--------==--=%%%%%%%%%%***%%**%%#%%%%%%%%%**%%%%%#%%%%%%%##",
		"#*****=------===-----------=---==-------------------#%%#%%%%%%%%***%%%#%%*#%%%%%%%%***%#%%#%%%%%%%%#",
			"#******------------------------------------#%#-----#%*=%%%%%%#%%*%%%%%%%#*#%%%%%%%%*****%#%%%%%%%%%%",
			"#*******=----=-------------------------=---*%%%*=-##+=#%%%%%%#%%#%%%%%%%#*#%%%%%%%%****#%#%%%%%%%%%#",
			"#********=----------------------------=+===*%%%%#==--*#%%%%%%*%%%%%%%%%%%*#%%%%#%%%*****#*%%%%%%%#*#",
			"#*********+--------------------------*%%*--*%%%%%*-=**%%%%%%%**##%%%%%%*%**%%%%#%%%#******%%%%%%%#**",
		 "#***********=-----=----------------=%%%%+++#%%%%%%****%%%%%%#****%%%%%%*%**#%%%*%%%%******%%%#%%%#**",
			"#**********#####**-----------------*%%%%+---#%%%%*****%%%%%%*****#%%%%#****%%%%*%%%%******%%%*%%%%**",
			"%#******#%*##******-==-------------%%%%*+----%%%%****%%%##%%******%%%%*****#%%%*%%%%#*****%%%**%%%*#",
			"%##***%%%%##****#***=--------------=#%%##****#%%%***%%%#*%%%******#%%%*****#%%%*%%%%#****#%%%**%%%##",
	  "%##******%#**#%%%##*****++==---+****%%%#*****%#%%**#%%%*******%%%*****#%%%*%%%%%****%%%**#%%%##",
		 "####*####%###**%#*#######***********%%%%****#%%%%**%%#***%%#******#%%%#*****%%%#*%%%#****%%#**#%%%%%",
		"%%%%###%#%%%%#%#%%%#%%%#%%%%%%#####%%#%#%###%%%%#%%#####%%%%##%%##%%%##%%%%%%%%%%%%%####%%%%%#%%%%%",
			"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%",
			"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%",
			"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%",
			"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
	};

	// Render ASCII art line by line with delay
	for (const auto& line : asciiArt) {
		centerText(line);
		std::cout << "\n";
		std::cout.flush();
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}

	std::cout << "\n";
	std::cout.flush();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	// Render top separator line
	centerText("=======================");
	std::cout << "\n";
	std::cout.flush();
	std::this_thread::sleep_for(std::chrono::milliseconds(80));

	// Type out the title character by character
	std::string titleLine = "|   O U T B R E A K   |";
	int paddingSpaces = (GameEngine::CONSOLE_WIDTH - (int)titleLine.length()) / 2;
	if (paddingSpaces < 0) paddingSpaces = 0;

	std::string padding(paddingSpaces, ' ');
	std::string displayedText = "";

	for (size_t i = 0; i < titleLine.length(); ++i) {
		displayedText += titleLine[i];
		std::cout << "\r" << padding << displayedText;
		std::cout.flush();
		std::this_thread::sleep_for(std::chrono::milliseconds(60));
	}

	std::cout << "\n";
	std::cout.flush();
	std::this_thread::sleep_for(std::chrono::milliseconds(80));

	// Render bottom separator line
	centerText("=======================");
	std::cout << "\n";
	std::cout.flush();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	std::cout << "\n";
	std::cout.flush();
}

void TitleScreen::drawAtmosphere() {
}

void TitleScreen::drawMenuOptions() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    const int firstMenuRow = 38;
    const int rowSpacing = 2;  // CHANGED from 1 to 2 to match actual spacing

    for (size_t i = 0; i < menuOptions.size(); ++i) {
        COORD pos = { 0, (SHORT)(firstMenuRow + i * rowSpacing) };
        SetConsoleCursorPosition(h, pos);
        if (i == selectedIndex)
            centerText("[>>] " + menuOptions[i] + " [<<]");
        else
            centerText("[ ] " + menuOptions[i]);
        // Print newline to move to next row
        std::cout << "\n";
    }

    lastRenderedIndex = selectedIndex;
}

void TitleScreen::drawFooter() {
}

void TitleScreen::renderMenu() {
	clearScreen();
	drawTitle();
	drawAtmosphere();
	
	// Render menu options with staggered appearance
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	const int firstMenuRow = 38;
	const int rowSpacing = 2;

	for (size_t i = 0; i < menuOptions.size(); ++i) {
		COORD pos = { 0, (SHORT)(firstMenuRow + i * rowSpacing) };
		SetConsoleCursorPosition(h, pos);
		if (i == selectedIndex)
			centerText("[>>] " + menuOptions[i] + " [<<]");
		else
			centerText("[ ] " + menuOptions[i]);
		std::cout << "\n";
		std::cout.flush();
		std::this_thread::sleep_for(std::chrono::milliseconds(150));
	}

	drawFooter();
	lastRenderedIndex = selectedIndex;
}

void TitleScreen::updateMenuSelection() {
#ifdef _WIN32
    if (lastRenderedIndex == selectedIndex)
        return; // nothing to do

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    const int firstMenuRow = 38;
    const int rowSpacing = 2;

    // Clear old selection
    if (lastRenderedIndex >= 0) {
        COORD oldPos = { 0, (SHORT)(firstMenuRow + lastRenderedIndex * rowSpacing) };
        SetConsoleCursorPosition(h, oldPos);
        std::string clearLine(GameEngine::CONSOLE_WIDTH, ' ');
        std::cout << clearLine;
        SetConsoleCursorPosition(h, oldPos);
        centerText("[ ] " + menuOptions[lastRenderedIndex]);
    }

    // Draw new selection
    COORD newPos = { 0, (SHORT)(firstMenuRow + selectedIndex * rowSpacing) };
    SetConsoleCursorPosition(h, newPos);
    std::string clearLine(GameEngine::CONSOLE_WIDTH, ' ');
    std::cout << clearLine;
    SetConsoleCursorPosition(h, newPos);
    centerText("[>>] " + menuOptions[selectedIndex] + " [<<]");

    std::cout.flush();
    lastRenderedIndex = selectedIndex;
#endif
}

void TitleScreen::handleInput() {
#ifdef _WIN32
    static bool upPressedLast = false;
    static bool downPressedLast = false;
    static bool enterPressedLast = false;

    bool upPressed = (GetAsyncKeyState(VK_UP) & 0x8000) != 0;
    bool downPressed = (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0;
    bool enterPressed = (GetAsyncKeyState(VK_RETURN) & 0x8000) != 0;

    if (upPressed && !upPressedLast) {
        if (selectedIndex > 0) {
            selectedIndex--;
            updateMenuSelection();
        }
    }
    else if (downPressed && !downPressedLast) {
        if (selectedIndex < (int)menuOptions.size() - 1) {
            selectedIndex++;
            updateMenuSelection();
        }
    }
    else if (enterPressed && !enterPressedLast) {
        isRunning = false;
    }

    upPressedLast = upPressed;
    downPressedLast = downPressed;
    enterPressedLast = enterPressed;

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
#endif
}

bool TitleScreen::initialize(const std::vector<std::string>& options) {
    if (options.empty()) {
        std::cerr << "[ERROR] No menu options.\n";
        return false;
    }
    menuOptions = options;
    selectedIndex = 0;
    lastRenderedIndex = -1;
    return true;
}

int TitleScreen::run() {
    isRunning = true;
    renderMenu();

    while (isRunning) {
        handleInput();
    }

    return selectedIndex;
}
