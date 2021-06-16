#ifndef INPUT_H
#define INPUT_H

enum class Command {UP = 0, DOWN, LEFT, RIGHT, PAUSE, END, ERROR, NONE};

Command Input();

#endif