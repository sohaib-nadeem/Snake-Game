#include "logic.h"

SnakeGame::SnakeGame(unsigned int seed): horizontal_period(42), vertical_period(90), start(), rng(seed),
    height(42), width(90), current_game_window(), view(NULL), fruit_index(), score(0), game_over(false),
    snake_indices(), dir(direction::STOP)
{
    // setup the game_window
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (i == 0 || i == height - 1 ||
             j == 0 || j == width - 1) {
                 current_game_window += '#';
             }
             else {
                 current_game_window += ' ';
             }
        }
        current_game_window += "\r\n";
    }

    // add the snake's head to the snake_indices and place it in the current_game_window
    snake_indices.push_back((height / 2) * (width + 2) + (width / 2));
    current_game_window[snake_indices[0]] = 'o';

    // place the fruit in a random position but not where the snake or wall is
    fruit_index = rng() % ((width + 2) * height); // replaced std::rand()
    while(current_game_window[fruit_index] != ' ') {
        fruit_index = rng() % ((width + 2) * height);
    }
    current_game_window[fruit_index] = 'F';
    
    // create view and update screen with initial setup
    view = new View{width, height, this};
    view->Update();
}

SnakeGame::~SnakeGame() {
    // free heap allocated memory
    delete view;
}

void SnakeGame::init() {
    Command command;
    // game loop 
    while(!game_over) {
        //frame_count++; // display which frame we're on, to be used in draw
        // Check for input
        command = Input();
        // Advance the position of the snake based on time
        //  and change the direction of the snake based input
        Logic(command);
        // Update Screen
        view->Update();
        //usleep(100000);
    }

    // Game Ends
}

void SnakeGame::Logic(Command command) {

    // If there is no command and snake is moving, continue game (i.e. advance snake)
    if (command == Command::NONE && dir != direction::STOP && move_snake()) {
        
        int index_of_next_pos_for_head = next_pos_in_direction(dir, snake_indices[0]); 
        
        // End game if snake is hitting itself
        if (current_game_window[index_of_next_pos_for_head] == 'o') {  
            game_over = true;
            dir = direction::STOP;
        }
        // Otherwise advance snake and make snake bigger if fruit is ahead (also spawning new fruit)
        else {
            // adjust snake_indices to advance snake 
            int snake_length = snake_indices.size();
            int old_last_index = snake_indices[snake_length - 1];
            for (int i = snake_length - 1; i >= 1; i--) { // mistake was here!!!!!!!!!!
                snake_indices[i] = snake_indices[i - 1];
            }
            snake_indices[0] = index_of_next_pos_for_head;

            // adjust game window appropriately
            current_game_window[index_of_next_pos_for_head] = head_char(dir);
            if (snake_length > 1) {
                current_game_window[snake_indices[1]] = 'o';
            }
            current_game_window[old_last_index] = ' ';

            // If fruit is to be consumed by the advance
            //  spawn new fruit and increase size of snake
            if (index_of_next_pos_for_head == fruit_index) {
                snake_indices.push_back(old_last_index);
                current_game_window[old_last_index] = 'o';
                fruit_index = rng() % ((width + 2) * height);
                while (current_game_window[fruit_index] != ' ') {
                    fruit_index = rng() % ((width + 2) * height);
                }
                current_game_window[fruit_index] = 'F';
            }
        }    
    }
    // If there is a command, change game state accordingly
    else if (command != Command::NONE) {
        // reset start when changing direction???
        start = std::chrono::high_resolution_clock::now();

        if (command == Command::ERROR) { // TODO: add proper error handling 
            throw;
        }
        else if (command == Command::UP && dir != direction::UP && dir != direction::DOWN) {
            dir = direction::UP;
            current_game_window[snake_indices[0]] = head_char(dir);
        }
        else if (command == Command::LEFT && dir != direction::LEFT && dir != direction::RIGHT) {
            dir = direction::LEFT;
            current_game_window[snake_indices[0]] = head_char(dir);
        }
        else if (command == Command::DOWN && dir != direction::UP && dir != direction::DOWN) {
            dir = direction::DOWN;
            current_game_window[snake_indices[0]] = head_char(dir);
        }
        else if (command == Command::RIGHT && dir != direction::LEFT && dir != direction::RIGHT) {
            dir = direction::RIGHT;
            current_game_window[snake_indices[0]] = head_char(dir);
        }
        else if (command == Command::END) {
            game_over = true;
            //dir = direction::STOP;??
        }

        //current_game_window[snake_indices[0]] = head_char(dir);
    }
    
}

std::string & SnakeGame::getNextGameWindow() {
    return current_game_window;
}

/*************** Private Helper Functions ****************/

bool SnakeGame::move_snake(void) {
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();

    std::chrono::milliseconds period;
    if (dir == direction::LEFT || dir == direction::RIGHT) {
        period = horizontal_period;
    }
    else if (dir == direction::UP || dir == direction::DOWN) {
        period = vertical_period;
    }

    if((now - start) > period) {
        start += period;
        return true;
    }
    else {
        return false;
    }
}

// helper function to get the index of the next position when moving in direction d
// if wall is ahead, gives the next position from the wall opposite to it
int SnakeGame::next_pos_in_direction(direction d, int current_pos) {
    int next_pos = -1;
    if (d == direction::UP) { 
        next_pos = current_pos - (width + 2);
        if (current_game_window[next_pos] == '#') {  
            next_pos += ((height - 3) * (width + 2));
        }
    }
    else if (d == direction::DOWN) {
        next_pos = current_pos + (width + 2);
        if (current_game_window[next_pos] == '#') {  
                next_pos -= ((height - 3) * (width + 2));
        }
    }
    else if (d == direction::LEFT) {
        next_pos = current_pos - 1;
        if (current_game_window[next_pos] == '#') {  
            next_pos += (width - 3);
        }
    }
    else if (d == direction::RIGHT) {
        next_pos = current_pos + 1;
        if (current_game_window[next_pos] == '#') {  
                next_pos -= (width - 3);
        }
    }

    return next_pos;
}

// helper function to get the appropriate character for the head in direction dir
char SnakeGame::head_char(direction d) {
    if (d == direction::UP) {
        return '^';
    }
    if (d == direction::DOWN) {
        return 'v';
    }
    if (d == direction::LEFT) {
        return '<';
    }
    if (d == direction::RIGHT) {
        return '>';
    }
    return 0; // should not reach here (this is just to make the code compile without warning) 
}
