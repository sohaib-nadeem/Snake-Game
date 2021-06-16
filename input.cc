#include "input.h"
#include <ncurses.h>

Command Input() {
    char ch = getch();
    if (ch == ERR) { // redundant since the else statement covers this case, but improves efficiency
        return Command::NONE;
    }
    else if (ch == 'w') {
        return Command::UP;
    }
    else if (ch == 'a') {
        return Command::LEFT;
    }
    else if (ch == 's') {
        return Command::DOWN;
    }
    else if (ch == 'd') {
        return Command::RIGHT;
    }
    else if (ch == 'p') {
        return Command::PAUSE;
    }
    else if (ch == 'e') {
        return Command::END;
    }
    else {
        return Command::NONE;
    }
}

