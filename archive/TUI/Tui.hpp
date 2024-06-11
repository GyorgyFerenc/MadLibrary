#pragma once

#include "../Core.hpp"

#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

struct Tui{
    int tty;

    struct termios old_termios;
};

proc init(Tui* tui){
    tui->tty = open("/dev/tty", O_RDWR);

    if (tcgetattr(0, &tui->old_termios) < 0) perror("init: Termios tcgetattr()");
    struct termios new_state = tui->old_termios;
    new_state.c_lflag &= ~ICANON;
    new_state.c_lflag &= ~ECHO;
    new_state.c_lflag &= ~ISIG;
    new_state.c_cc[VMIN] = 1;
    new_state.c_cc[VTIME] = 0;

    if (tcsetattr(0, TCSANOW, &new_state) < 0) perror("init: Termios tcsetattr()");
}

proc deinit(Tui* tui){
    if (tcsetattr(0, TCSADRAIN, &tui->old_termios) < 0) perror("deinit: Termios tcsetattr");
}

