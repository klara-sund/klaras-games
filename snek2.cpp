#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>
#include <random>

// ANSI escape codes for terminal manipulation
// These codes are a standard way to control the cursor and colors in a Unix terminal.
#define CLEAR_SCREEN "\033[2J"
#define CURSOR_HOME "\033[H"
#define RED_TEXT "\033[31m"
#define GREEN_TEXT "\033[32m"
#define RESET_COLOR "\033[0m"

// Define the characters for our snake and food.
const char FOOD_CHAR = '@'; // Changed for better visibility
const char SNAKE_HEAD_CHAR = 'O'; // Head of the snake
const char SNAKE_BODY_CHAR = 'o'; // Body of the snake
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
// Now takes a vector of points to represent the entire snake body.
void drawWorld(const std::vector<std::vector<char>>& world, const std::vector<Point>& snakeBody) {
    clearScreen(); // Always clear the screen before redrawing to prevent flickering

    // Create a temporary world to draw the snake on top of the food.
    std::vector<std::vector<char>> displayWorld = world;

    // Draw the snake body.
    if (!snakeBody.empty()) {
        // Draw the head first.
        displayWorld[snakeBody[0].y][snakeBody[0].x] = SNAKE_HEAD_CHAR;

        // Draw the rest of the body.
        for (size_t i = 1; i < snakeBody.size(); ++i) {
            displayWorld[snakeBody[i].y][snakeBody[i].x] = SNAKE_BODY_CHAR;
        }
    }

    // Print the border and the contents of the display world.
    std::cout << "--------------------" << std::endl;
    for (size_t y = 0; y < displayWorld.size(); ++y) {
        std::cout << "| "; // Left border
        for (size_t x = 0; x < displayWorld[y].size(); ++x) {
            if (displayWorld[y][x] == FOOD_CHAR) {
                std::cout << RED_TEXT << displayWorld[y][x] << RESET_COLOR;
            } else if (displayWorld[y][x] == SNAKE_HEAD_CHAR) {
                std::cout << GREEN_TEXT << displayWorld[y][x] << RESET_COLOR;
            } else if (displayWorld[y][x] == SNAKE_BODY_CHAR) {
                std::cout << GREEN_TEXT << displayWorld[y][x] << RESET_COLOR;
            } else {
                std::cout << displayWorld[y][x];
            }
        }
        std::cout << " |" << std::endl; // Right border
    }
    std::cout << "--------------------" << std::endl;
    std::cout << "Snake length: " << snakeBody.size() << std::endl;
}

// Function to find the nearest food item to the snake's head.
Point findNearestFood(const std::vector<std::vector<char>>& world, const Point& snakeHead) {
    Point nearestFood = {-1, -1};
    double minDistance = -1;

    for (size_t y = 0; y < world.size(); ++y) {
        for (size_t x = 0; x < world[y].size(); ++x) {
            if (world[y][x] == FOOD_CHAR) {
                // Calculate the Euclidean distance.
                double distance = std::sqrt(std::pow(x - snakeHead.x, 2) + std::pow(y - snakeHead.y, 2));
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
    if (foodPos.x != -1) {
        if (foodPos.x > snakePos.x) {
            nextPos.x++;
        } else if (foodPos.x < snakePos.x) {
            nextPos.x--;
        }

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

// Function to place food randomly on the grid.
void placeFoodRandomly(std::vector<std::vector<char>>& world, const std::vector<Point>& snakeBody) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disX(0, world[0].size() - 1);
    std::uniform_int_distribution<> disY(0, world.size() - 1);

    int foodCount = 0;
    while (foodCount < FOOD_COUNT) {
        int x = disX(gen);
        int y = disY(gen);

        // Check if the spot is empty and not part of the snake
        bool isSnake = false;
        for (const auto& segment : snakeBody) {
            if (segment.x == x && segment.y == y) {
                isSnake = true;
                break;
            }
        }

        if (world[y][x] == ' ' && !isSnake) {
            world[y][x] = FOOD_CHAR;
            foodCount++;
        }
    }
}

int main() {
    // Define the dimensions of our game world.
    const int WORLD_WIDTH = 20;
    const int WORLD_HEIGHT = 10;

    // Create our 2D game world as a vector of vectors of characters.
    std::vector<std::vector<char>> world(WORLD_HEIGHT, std::vector<char>(WORLD_WIDTH, ' '));
    
    // The snake is now a vector of points, representing each segment.
    std::vector<Point> snakeBody = {{5, 5}, {5, 6}, {5, 7}}; // Initial snake with 3 segments.

    // Place some "food" characters randomly in the world.
    placeFoodRandomly(world, snakeBody);

    // The main game loop.
    while (true) {
        // Get the current position of the snake's head.
        Point currentHead = snakeBody.front();
        
        // Find the nearest food item for the snake to chase.
        Point foodLocation = findNearestFood(world, currentHead);

        // Calculate the snake's next position based on the food location.
        Point nextHeadPosition = getNextMove(currentHead, foodLocation);

        // Add the new head position to the front of the snake's body vector.
        snakeBody.insert(snakeBody.begin(), nextHeadPosition);

        // Check if the snake has "eaten" the food.
        bool ateFood = (foodLocation.x != -1 && nextHeadPosition.x == foodLocation.x && nextHeadPosition.y == foodLocation.y);
        
        if (ateFood) {
            // If the snake ate food, remove the food from the world.
            world[foodLocation.y][foodLocation.x] = ' ';
            // Don't remove the tail, so the snake grows.
            std::cout << "The snake ate the food! It will now find the next one." << std::endl;
        } else {
            // If the snake didn't eat, remove the last segment of the tail.
            snakeBody.pop_back();
        }

        // Draw the updated world.
        drawWorld(world, snakeBody);

        // Pause for a short duration to control the speed of the animation.
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    return 0;
}
