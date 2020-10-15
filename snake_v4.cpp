#include <iostream>
#include <ncurses.h>
#include <ctime>
#include <vector>
#include <string>

#include <thread>
#include <chrono>
#include <unistd.h>

std::chrono::milliseconds horizontal_period(42);
std::chrono::milliseconds vertical_period(90);
std::chrono::high_resolution_clock::time_point start;

const int height = 42;
const int width = 90;
//int frame_count = 0;


std::string previous_game_window((width + 2) * height, 'x');
std::string current_game_window;

std::vector<int> snake_indices;
int fruit_index;

int score;
enum direction {STOP = 0, UP = 1, DOWN = -1, LEFT = -2, RIGHT = 2, NO_DIRECTION = 3};
direction dir;
bool game_over;

void Draw(void) {

    //system("clear");
    //std::cout << current_game_window /*<< time(NULL) << "\r\n" << frame_count*/;

    // update game by comparing previous_game_window to current_game_window
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if(previous_game_window[(width + 2) * y + x] != current_game_window[(width + 2) * y + x]) {
                move(y, x);
                addch(current_game_window[(width + 2) * y + x]);
            }
        }
    }
    move(120, 56); // move outside game_window
    refresh();
    
    // update previous_game_window to prepare for next cycle of the game loop
    previous_game_window = current_game_window;
}

void Setup() {
    game_over = false;
    dir = STOP;
    score = 0;

    // setup the current_game_window
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
    fruit_index = std::rand() % ((width + 2) * height);
    while(current_game_window[fruit_index] != ' ') {
        fruit_index = std::rand() % ((width + 2) * height);
    }
    current_game_window[fruit_index] = 'F';


    // display current_game_window and set previous_game_window equal to current_game_window
    // to be able to start updating the game via the game_loop
    system("clear");

    Draw();
    //refresh();

    //previous_game_window = current_game_window;
}


direction Input() {
    char ch = getch();
    if (ch == ERR) { // redundant since the else statement covers this case, but improves efficiency
        return NO_DIRECTION;
    }
    else if (ch == 'w' && dir != UP  && dir != DOWN) {
        return UP;
    }
    else if (ch == 'a' && dir != LEFT  && dir != RIGHT) {
        return LEFT;
    }
    else if (ch == 's' && dir != UP  && dir != DOWN) {
        return DOWN;
    }
    else if (ch == 'd' && dir != LEFT  && dir != RIGHT) {
        return RIGHT;
    }
    /*else if (ch == 'e') {
        return END_GAME;
    }*/
    else {
        return NO_DIRECTION;
    }
}

// helper function to get the index of the next position when moving in direction d
// if wall is ahead, gives the next position from the wall opposite to it
int next_pos_in_direction (direction d, int current_pos) {
    int next_pos;
    if (dir == UP) { 
        next_pos = current_pos - (width + 2);
        if (current_game_window[next_pos] == '#') {  
            next_pos += ((height - 3) * (width + 2));
        }
    }
    else if (dir == DOWN) {
        next_pos = current_pos + (width + 2);
        if (current_game_window[next_pos] == '#') {  
                next_pos -= ((height - 3) * (width + 2));
        }
    }
    else if (dir == LEFT) {
        next_pos = current_pos - 1;
        if (current_game_window[next_pos] == '#') {  
            next_pos += (width - 3);
        }
    }
    else if (dir == RIGHT) {
        next_pos = current_pos + 1;
        if (current_game_window[next_pos] == '#') {  
                next_pos -= (width - 3);
        }
    }

    return next_pos;
}

// helper function to get the appropriate character for the head in direction dir
char head_char(direction d) {
    if (dir == UP) {
        return '^';
    }
    if (dir == DOWN) {
        return 'v';
    }
    if (dir == LEFT) {
        return '<';
    }
    if (dir == RIGHT) {
        return '>';
    }
    return 0; // should not reach here (this is just to make the code compile without warning) 
}

bool move_snake(void) {
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();

    std::chrono::milliseconds period;
    if (dir == LEFT || dir == RIGHT) {
        period = horizontal_period;
    }
    else if (dir == UP || dir == DOWN) {
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

void Logic(direction input_dir) {

    if (input_dir == NO_DIRECTION && dir != STOP && move_snake()) { // continue game, i.e. advance snake
        
        int index_of_next_pos_for_head = next_pos_in_direction(dir, snake_indices[0]); 
        
        if (current_game_window[index_of_next_pos_for_head] == 'o') {  // end game if snake is hitting itself
            game_over = true;
            dir = STOP;
        }
        else { // otherwise advance snake and make snake bigger if fruit is ahead (also spawning new fruit)

            // adjust snake_indices and current_game_window, spawning new fruit if old fruit is to be consumed
            int snake_length = snake_indices.size();
            int old_last_index = snake_indices[snake_length - 1];
            for (int i = snake_length - 1; i >= 1; i--) { // mistake was here!!!!!!!!!!
                snake_indices[i] = snake_indices[i - 1];
            }
            snake_indices[0] = index_of_next_pos_for_head;
            
            
            if (index_of_next_pos_for_head == fruit_index) {
                current_game_window[index_of_next_pos_for_head] = head_char(dir);
                snake_indices.push_back(old_last_index);
                current_game_window[snake_indices[1]] = 'o'; // no need to check that snake_length is at least 1                
                // snake_length++; // update to use for re-rendering
                // spawn new fruit
                fruit_index = std::rand() % ((width + 2) * height);
                while (current_game_window[fruit_index] != ' ') {
                    fruit_index = std::rand() % ((width + 2) * height);
                }
                current_game_window[fruit_index] = 'F';
            }
            else {
                current_game_window[index_of_next_pos_for_head] = head_char(dir);
                if (snake_length > 1) {
                    current_game_window[snake_indices[1]] = 'o';
                }
                current_game_window[old_last_index] = ' ';
            }

        }    

    }
    else if (input_dir != NO_DIRECTION) {
        // reset start when changing direction
        start = std::chrono::high_resolution_clock::now();

        dir = input_dir;
        current_game_window[snake_indices[0]] = head_char(dir);
    }
    
}


// main program: setup, game loop 

int main() {
    
    srand(time(NULL)); // set the seed for the random no. generator using the clock

    initscr();
    cbreak();
    noecho(); //does some wacky stuff when used with iostream functions!!!! dont'use ncurses with iostream functions (std::cout)
    nodelay(stdscr, TRUE);
    
    direction in;
    Setup();
    while(!game_over) {
        //frame_count++; // display which frame we're on, to be used in draw
        in = Input();
        Logic(in);
        Draw();
        //usleep(100000);
    }



    endwin();
    return 0;
}