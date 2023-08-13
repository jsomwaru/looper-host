#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <ncurses.h>
#include <string.h>


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
        // char channel_status[18];
        // std::sprintf(channel_status, "Active Channel %d", channel_count);
        // printw(channel_status);
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