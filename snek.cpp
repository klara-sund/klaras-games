#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>

// ANSI escape codes for terminal manipulation
// These codes are a standard way to control the cursor and colors in a Unix terminal.
// They are more portable than platform-specific functions for a self-contained program.
#define CLEAR_SCREEN "\033[2J"
#define CURSOR_HOME "\033[H"
#define RED_TEXT "\033[31m"
#define GREEN_TEXT "\033[32m"
#define RESET_COLOR "\033[0m"

// Define a struct to hold the dimensions of our ASCII character art.
// This makes it easy to change the size of the snake or other NPCs later.
struct Dimensions {
    int width;
    int height;
};

// Define the ASCII characters for our snake.
// The user requested a 3x12 snake, which we'll represent as an array of strings.
// A string is an array of characters, so this is a 2D array of chars.
const std::vector<std::string> SNAKE_BODY = {
    "            ",
    "  o - - - - ",
    "            "
};
const Dimensions SNAKE_DIMS = {12, 3};

// Define the character for the target 'food' the snake will seek.
const char FOOD_CHAR = 'o';
const int FOOD_COUNT = 5;

// A simple struct to represent a point on the grid.
struct Point {
    int x;
    int y;
};

// Function to clear the terminal screen using ANSI escape codes.
void clearScreen() {
    std::cout << CLEAR_SCREEN << CURSOR_HOME;
}

// Function to draw the game world, including the snake and food.
void drawWorld(const std::vector<std::vector<char>>& world, const Point& snakePos) {
    clearScreen(); // Always clear the screen before redrawing to prevent flickering

    // Print the border of the world.
    std::cout << "--------------------" << std::endl;

    // Loop through each row and column of our 2D world array.
    for (size_t y = 0; y < world.size(); ++y) {
        std::cout << "| "; // Left border

        // Loop through each character in the row.
        for (size_t x = 0; x < world[y].size(); ++x) {
            bool isSnake = false;
            // Check if the current position (x, y) is part of the snake.
            // We use the snake's top-left corner (snakePos) and its dimensions
            // to check if a character should be drawn.
            if (x >= snakePos.x && x < snakePos.x + SNAKE_DIMS.width &&
                y >= snakePos.y && y < snakePos.y + SNAKE_DIMS.height) {
                // Get the corresponding character from our SNAKE_BODY array.
                char snakeChar = SNAKE_BODY[y - snakePos.y][x - snakePos.x];
                if (snakeChar != ' ') {
                    std::cout << GREEN_TEXT << snakeChar << RESET_COLOR;
                    isSnake = true;
                }
            }
            // If the position is not part of the snake, draw the world character.
            if (!isSnake) {
                if (world[y][x] == FOOD_CHAR) {
                    // If the character is food, make it red.
                    std::cout << RED_TEXT << world[y][x] << RESET_COLOR;
                } else {
                    std::cout << world[y][x];
                }
            }
        }
        std::cout << " |" << std::endl; // Right border
    }
    std::cout << "--------------------" << std::endl;
    std::cout << "Snake position: (" << snakePos.x << ", " << snakePos.y << ")" << std::endl;
}

// Function to find the nearest food item to the snake.
// This is a simple, brute-force search algorithm.
Point findNearestFood(const std::vector<std::vector<char>>& world, const Point& snakePos) {
    Point nearestFood = {-1, -1};
    double minDistance = -1;

    for (size_t y = 0; y < world.size(); ++y) {
        for (size_t x = 0; x < world[y].size(); ++x) {
            if (world[y][x] == FOOD_CHAR) {
                // Calculate the Euclidean distance between the snake and the food.
                double distance = std::sqrt(std::pow(x - snakePos.x, 2) + std::pow(y - snakePos.y, 2));

                // If this is the first food found or it's closer than the previous one,
                // update the nearest food location.
                if (nearestFood.x == -1 || distance < minDistance) {
                    minDistance = distance;
                    nearestFood = {static_cast<int>(x), static_cast<int>(y)};
                }
            }
        }
    }
    return nearestFood;
}

// Function to calculate the snake's next move based on the nearest food.
Point getNextMove(const Point& snakePos, const Point& foodPos) {
    Point nextPos = snakePos;
    
    // Simple greedy algorithm: move one step closer to the food on the x-axis, then the y-axis.
    if (foodPos.x != -1) { // Check if food was found
        if (foodPos.x > snakePos.x) {
            nextPos.x++;
        } else if (foodPos.x < snakePos.x) {
            nextPos.x--;
        }

        // If we haven't moved on the x-axis, move on the y-axis.
        if (nextPos.x == snakePos.x) {
            if (foodPos.y > snakePos.y) {
                nextPos.y++;
            } else if (foodPos.y < snakePos.y) {
                nextPos.y--;
            }
        }
    }
    return nextPos;
}

int main() {
    // Define the dimensions of our game world.
    const int WORLD_WIDTH = 20;
    const int WORLD_HEIGHT = 10;

    // Create our 2D game world as a vector of vectors of characters.
    std::vector<std::vector<char>> world(WORLD_HEIGHT, std::vector<char>(WORLD_WIDTH, ' '));
    
    // Place some "food" characters randomly in the world.
    // NOTE: This is a simple placement. We could improve this later to avoid placing food on the snake.
    world[3][5] = FOOD_CHAR;
    world[8][15] = FOOD_CHAR;
    world[2][10] = FOOD_CHAR;
    world[6][3] = FOOD_CHAR;
    world[7][18] = FOOD_CHAR;

    // Set the initial position of the snake.
    Point snakePosition = {1, 1};

    // The main game loop. This runs indefinitely until the user closes the program.
    while (true) {
        // Find the nearest food item for the snake to chase.
        Point foodLocation = findNearestFood(world, snakePosition);

        // Calculate the snake's next position based on the food location.
        Point nextPosition = getNextMove(snakePosition, foodLocation);

        // Update the snake's position.
        snakePosition = nextPosition;

        // Check if the snake has "eaten" the food.
        // We'll consider it eaten if the snake's head (top-left corner) is on the food's location.
        // This is a simple collision detection. We can make it more sophisticated later.
        if (foodLocation.x != -1 && 
            snakePosition.x == foodLocation.x && 
            snakePosition.y == foodLocation.y) {
            
            // Remove the food from the world.
            world[foodLocation.y][foodLocation.x] = ' ';
            std::cout << "The snake ate the food! It will now find the next one." << std::endl;
        }

        // Draw the updated world.
        drawWorld(world, snakePosition);

        // Pause for a short duration to control the speed of the animation.
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    return 0;
}
