#ifndef LOGIC_H
#define LOGIC_H

#include <vector>
#include <string>
#include <chrono>
#include <random>

#include "input.h"
#include "drawer.h"

//unused
struct position {
    int x;
    int y;
};

enum class direction {STOP = 0, UP = 1, DOWN = -1, LEFT = -2, RIGHT = 2, NO_DIRECTION = 3};

class SnakeGame {
    private:    
    const std::chrono::milliseconds horizontal_period;
    const std::chrono::milliseconds vertical_period;
    std::chrono::high_resolution_clock::time_point start;
    //int frame_count = 0;
    std::default_random_engine rng;

    const int height;
    const int width;
    //std::string previous_game_window;
    std::string current_game_window;
    int fruit_index;
    int score;
    bool game_over;
    std::vector<int> snake_indices;
    direction dir;

    View * view;

    /*** Public Methods ***/
    public:
    SnakeGame(unsigned int seed);
    virtual ~SnakeGame();
    void init();
    std::string & getNextGameWindow();
    /*** Private Methods ***/
    private:
    void Logic(Command command);
    bool move_snake(void);
    int next_pos_in_direction(direction d, int current_pos);
    char head_char(direction d);
};

#endif