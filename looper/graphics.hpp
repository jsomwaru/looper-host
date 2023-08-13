#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <ncurses.h>


static bool inited = false;

void setup_display() {
    initscr();
    inited = true;
}

void display_recording(bool recording) {
    if(!inited)
        setup_display();
    if (recording) {
        clear();
        printw("Recording\n");
    }
    else {
        clear();
        printw("Not Recording\n");
    }
    refresh();
}

void cleanup_display() {
    endwin();
}

#endif