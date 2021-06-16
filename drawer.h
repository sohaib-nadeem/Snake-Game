#ifndef DRAWER_H
#define DRAWER_H

#include <string>

class SnakeGame;

class View {
    private:
    int width;
    int height;
    std::string game_window;
    SnakeGame * gameState;
    
    public:
    View(int width, int height, SnakeGame * gameState);
    ~View();
    void Update();
};

#endif