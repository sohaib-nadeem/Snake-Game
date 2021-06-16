#include "logic.h"

// current includes: ncurses.h, vector, string, chrono, random
// removed includes: ctime, unistd.h
// future include: thread (for multithreading)

// Main Program: create and initialise game

// Bugs To Fix:
//       1) bug: changing directions twice quickly makes the snake hit itself  
//          fix: add a variable that stores the previous movement

int main() { 
    // set the seed for the random no. generator using the clock
    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count(); // replaced: srand(time(NULL));
    SnakeGame game{seed};
    game.init();

    return 0;
}