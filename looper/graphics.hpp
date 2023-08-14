#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <ncurses.h>
#include <string.h>


static bool inited = false;

void setup_display() {
    initscr();
    inited = true;
}

void display_recording(bool recording, int active_channel) {
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
    char channel_status[18];
    std::sprintf(channel_status, "Active Channel %d", active_channel);
    printw(channel_status);
    refresh();
}

void cleanup_display() {
    endwin();
}

#endif