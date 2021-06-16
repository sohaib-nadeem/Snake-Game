#include "drawer.h"
#include "logic.h"
#include <ncurses.h>

View::View(int width, int height, SnakeGame * gameState): width{width}, height{height}, 
    game_window(height * (width + 2), 'x'), gameState{gameState}
{
    initscr();
    cbreak();
    noecho(); //does some wacky stuff when used with iostream functions!!!! dont'use ncurses with iostream functions (std::cout)
    nodelay(stdscr, TRUE);
}

View::~View() {
    endwin();
}

// could add frame time/frame rate, score
void View::Update() {
    std::string & next_game_window = gameState->getNextGameWindow();

    // update game by comparing previous_game_window to current_game_window
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (game_window[(width + 2) * y + x] != next_game_window[(width + 2) * y + x]) {
                move(y, x);
                addch(next_game_window[(width + 2) * y + x]);
                // update game_window to prepare for next cycle of the game loop
                game_window[(width + 2) * y + x] = next_game_window[(width + 2) * y + x];
            }
        }
    }

    // move outside game_window
    move(120, 56);
    // update screen
    refresh();
}