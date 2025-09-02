import os
import random
import time

# Define the game map as a list of lists
# '@' is the player, 'M' is a monster, 'E' is the exit, '.' is a path, '#' is a wall
# You can easily edit this map to create new levels
MAP = [
    "#######E#####",
    "#...........#",
    "#.#.##.##.###",
    "#.#...#...#M#",
    "#.###.#.##.##",
    "#...#.#.#...#",
    "#.##.###.##.#",
    "#.#.#.#.#.#.#",
    "#.#.###.#.#.#",
    "#.#...#...#.#",
    "#.#####.#####",
    "#.@.........#",
    "#############",
]

# Get the dimensions of the map
MAP_HEIGHT = len(MAP)
MAP_WIDTH = len(MAP[0])

# Global game state variables
player_x, player_y = 1, 11
monster_x, monster_y = 11, 3
game_over = False
win_condition = False

# Function to clear the terminal screen
def clear_screen():
    # Use 'cls' for Windows, 'clear' for Linux/macOS
    os.system('cls' if os.name == 'nt' else 'clear')

# Function to draw the entire game state to the terminal
def draw_map():
    clear_screen()
    
    # We create a temporary copy of the map to draw the player and monster on
    display_map = [list(row) for row in MAP]

    # Place the player and monster characters on the map
    display_map[player_y][player_x] = '@'
    display_map[monster_y][monster_x] = 'M'

    # Print the map row by row
    for row in display_map:
        print("".join(row))

# Function to handle player movement
def move_player(direction):
    global player_x, player_y, game_over, win_condition
    
    new_x, new_y = player_x, player_y
    
    # Update new coordinates based on the direction
    if direction == 'w':
        new_y -= 1
    elif direction == 's':
        new_y += 1
    elif direction == 'a':
        new_x -= 1
    elif direction == 'd':
        new_x += 1
    
    # Check if the new position is a wall ('#')
    if MAP[new_y][new_x] == '#':
        print("You can't move there!")
        time.sleep(1) # Pause for 1 second to show the message
        return
        
    # Check for win condition (reaching 'E')
    if MAP[new_y][new_x] == 'E':
        win_condition = True
        return

    # Update player position if the move is valid
    player_x, player_y = new_x, new_y

    # Check for collision with monster
    if player_x == monster_x and player_y == monster_y:
        game_over = True

# Function to handle monster movement (simple random movement)
def move_monster():
    global monster_x, monster_y
    
    # The monster randomly chooses a direction
    direction = random.choice(['w', 'a', 's', 'd'])
    new_x, new_y = monster_x, monster_y

    if direction == 'w':
        new_y -= 1
    elif direction == 's':
        new_y += 1
    elif direction == 'a':
        new_x -= 1
    elif direction == 'd':
        new_x += 1

    # Check if the new position is a wall and update if not
    if MAP[new_y][new_x] != '#':
        monster_x, monster_y = new_x, new_y
        
    # Check for collision with player after monster moves
    if player_x == monster_x and player_y == monster_y:
        global game_over
        game_over = True

# Main game loop
def main():
    global game_over, win_condition
    
    # The main loop continues as long as the game is not over
    while not game_over and not win_condition:
        draw_map()
        
        # Display instructions and get input from the user
        print("\nUse WASD to move. Find the 'E' to escape the monster!")
        
        # This is a simple way to get input without external libraries.
        # It waits for the user to press a key and then 'Enter'.
        # For a snappier experience, you'd use a library like `curses` or `pynput`,
        # but this keeps the code simple and self-contained.
        user_input = input("Move: ").lower()
        
        if user_input in ['w', 'a', 's', 'd']:
            move_player(user_input)
            move_monster()
        else:
            print("Invalid move. Use w, a, s, or d.")
            time.sleep(1)

    # After the loop, print the final game state and result
    clear_screen()
    draw_map()
    if win_condition:
        print("\n\nCongratulations! You escaped the dungeon!")
    else:
        print("\n\nGame Over! The monster caught you.")

# Start the game when the script is run
if __name__ == "__main__":
    main()
