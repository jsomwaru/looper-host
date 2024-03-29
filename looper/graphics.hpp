#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <ncurses.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "channel.hpp"


static bool inited = false;

static struct termios old_tio;

void setup_display() {
    initscr();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    inited = true;
}

void display_recording(bool recording, bool recorded, bool paused, int active_channel) {
    if(!inited)
        setup_display();
    if (recording) {
        clear();
        attron(COLOR_PAIR(1));
        printw("Recording\n");
        attroff(COLOR_PAIR(1));
    }
    else {
        clear();
        attron(COLOR_PAIR(2));
        printw("Not Recording\n");
        attroff(COLOR_PAIR(2));
    }
    printw("Recorded: %s\n", recorded ? "True" : "False");
    printw("Paused: %s\n", paused ? "True" : "False");
    printw("Active Channel %d\n", active_channel);
    refresh();
}

void cleanup_display() {
    endwin();
}

void configure_terminal() {
    struct termios new_tio;
    tcgetattr(STDIN_FILENO, &old_tio);
    /* we want to keep the old setting to restore them a the end */
    new_tio=old_tio;
    /* disable canonical mode (buffered i/o) and local echo */
    new_tio.c_lflag &=(~ICANON & ~ECHO);
    /* set the new settings immediately */
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
    setup_display();
}

int get_user_input(ChannelRack &master) {
    while (1) {
        display_recording(master.get_active_channel().recording, master.get_active_channel().recorded, master.paused, master.active_channel);
        unsigned char c = getchar();
        if (c == ' ') {
            if (master.get_active_channel().recorded && !master.get_active_channel().recording) {
                // need to stop recording before clearing 
                master.get_active_channel().recording = !master.get_active_channel().recording;
                master.get_active_channel().recorded = false;
                master.get_active_channel().clear();
            }
            master.get_active_channel().recording = !master.get_active_channel().recording;
        } else if (c == 'b') {
            if (master.get_active_channel().recording)
                master.get_active_channel().recording = false;
            master.decrement_active_channel();
        } else if (c == 'n') {
            if (master.get_active_channel().recording)
                master.get_active_channel().recording = false;
            master.increment_active_channel();
        } else if (c == 'p') {
            master.play_pause();
        }
    }
    return 0;
}

#endif