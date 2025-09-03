#include <iostream>
#include <vector>
#include <string>

// --- Cross-platform includes for console input and sleep ---
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <stdio.h>
#endif

// --- Constants for game elements ---
const char WALL = '#';
const char ROBBER = 'R';
const char COP = 'C';
const char COLLECTIBLE = 'o';
const char EMPTY = ' ';

// --- ANSI color codes for terminal coloring ---
#ifdef _WIN32
// Note: Windows Command Prompt may not support ANSI codes by default.
// This is a basic setup; more robust solutions exist.
const char* RESET_COLOR = "";
const char* RED_COLOR = "";
const char* BLUE_COLOR = "";
const char* YELLOW_COLOR = "";
const char* GRAY_COLOR = "";
#else
const char* RESET_COLOR = "\033[0m";
const char* RED_COLOR = "\033[31m";
const char* BLUE_COLOR = "\033[34m";
const char* YELLOW_COLOR = "\033[33m";
const char* GRAY_COLOR = "\033[37m";
#endif

// --- Game state variables ---
std::vector<std::string> gameMap = {
    "#####################################",
    "#o o o o o o o o o o o o o o o o o o#",
    "#o ########### #o# # #############o#",
    "#  #         # #o# # #           # #",
    "#o # ####### # # #o# # ########### #",
    "#  # #     # # # # #o# #         #o#",
    "#o # # ### # # # ### #o# ####### # #",
    "#  # # # # # # # #   # #o#     # # #",
    "#o # # # #o# # #o### # # # ### #o#o#",
    "#  # # #o# # # # # # # # # # # # #o#",
    "#  ### # # # #o# # # # # #o# ### #o#",
    "#o o o # #o# # # # # # #o# # #   # #",
    "# # #o# #o# # #o# #o# # # # # #o#o#",
    "# # # #o#o# # # # # # # #o# # # # #",
    "# #o# # # #o# # # # # # # #o# # # #",
    "# # # # # # # # # # # # # # # # #o#",
    "# # # # # #o# # # # # # # # # # # #",
    "#o o o o o o o o o o o o o o o o o o#",
    "#####################################"
};

int robberX = 1;
int robberY = 1;
int copX = 33;
int copY = 17;
int score = 0;
int collectiblesLeft = 32; // Updated count for the new map
bool gameOver = false;

// Function to move the cursor to a specific position (x, y)
void gotoxy(int x, int y) {
#ifdef _WIN32
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    printf("\033[%d;%dH", y, x);
#endif
}

// Function to print the game map to the console
void drawMap() {
    // Move cursor to top-left to redraw without clearing
    gotoxy(1, 1);

    std::cout << "Score: " << score << " | Collectibles Left: " << collectiblesLeft << std::endl;
    for (const auto& row : gameMap) {
        for (char c : row) {
            if (c == ROBBER) {
                std::cout << RED_COLOR << c << RESET_COLOR;
            } else if (c == COP) {
                std::cout << BLUE_COLOR << c << RESET_COLOR;
            } else if (c == COLLECTIBLE) {
                std::cout << YELLOW_COLOR << c << RESET_COLOR;
            } else if (c == WALL) {
                std::cout << GRAY_COLOR << c << RESET_COLOR;
            } else {
                std::cout << c;
            }
        }
        std::cout << std::endl;
    }
}

// Function to handle player movement
void moveRobber(char move) {
    int newX = robberX;
    int newY = robberY;

    if (move == 'w' || move == 'W') newY--;
    else if (move == 's' || move == 'S') newY++;
    else if (move == 'a' || move == 'A') newX--;
    else if (move == 'd' || move == 'D') newX++;

    // Check for collisions
    if (newY >= 0 && newY < gameMap.size() && newX >= 0 && newX < gameMap[0].size() && gameMap[newY][newX] != WALL) {
        // Clear old position
        gameMap[robberY][robberX] = EMPTY;
        robberX = newX;
        robberY = newY;

        // Check for collectibles
        if (gameMap[robberY][robberX] == COLLECTIBLE) {
            score++;
            collectiblesLeft--;
            // Remove collectible by setting the position to empty
            gameMap[robberY][robberX] = EMPTY;
        }

        // Update the robber's position on the map
        gameMap[robberY][robberX] = ROBBER;
    }
}

// Simple AI for the cop: chase the robber
void moveCop() {
    // Clear old cop position
    if (copX >= 0 && copY >= 0 && copY < gameMap.size() && copX < gameMap[0].size()) {
        gameMap[copY][copX] = EMPTY;
    }

    if (copX < robberX) copX++;
    else if (copX > robberX) copX--;
    
    if (copY < robberY) copY++;
    else if (copY > robberY) copY--;

    // Check if cop and robber are in the same spot
    if (copX == robberX && copY == robberY) {
        gameOver = true;
    }

    // Update cop's position on the map
    if (copY >= 0 && copY < gameMap.size() && copX >= 0 && copX < gameMap[0].size()) {
        gameMap[copY][copX] = COP;
    }
}

// Main game loop
void gameLoop() {
    char input;
    
    // Initial setup on the map
    gameMap[robberY][robberX] = ROBBER;
    gameMap[copY][copX] = COP;

    // --- Cross-platform function for non-blocking input ---
#ifndef _WIN32
    // Setup for non-blocking input on Unix-like systems
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif

    while (!gameOver && collectiblesLeft > 0) {
        drawMap();
        
        // Wait for user input (non-blocking)
#ifdef _WIN32
        if (_kbhit()) {
            input = _getch();
            moveRobber(input);
        }
#else
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        struct timeval timeout = {0, 0};
        if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &timeout) > 0) {
            read(STDIN_FILENO, &input, 1);
            moveRobber(input);
        }
#endif

        moveCop();

        // Check if the robber has been caught
        if (robberX == copX && robberY == copY) {
            gameOver = true;
        }

        // Wait for a moment to slow down the game
#ifdef _WIN32
        Sleep(200);
#else
        usleep(200000); // 200 milliseconds
#endif
    }

#ifndef _WIN32
    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    // End game screen
    drawMap();
    if (collectiblesLeft == 0) {
        std::cout << "\nCongratulations! You collected all the items and won!" << std::endl;
    } else {
        std::cout << "\nGame Over! The cops caught the robber!" << std::endl;
    }
    std::cout << "Final Score: " << score << std::endl;
}

int main() {
    std::cout << "Welcome to Cops and Robbers!" << std::endl;
    std::cout << "Use W, A, S, D to move. Collect all the 'o's without getting caught!" << std::endl;
    std::cout << "Press any key to start..." << std::endl;
#ifdef _WIN32
    _getch();
#else
    char c;
    read(STDIN_FILENO, &c, 1);
#endif
    gameLoop();
    return 0;
}
