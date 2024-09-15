#include "platform.h"
#include <stdbool.h>

#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

struct {
    struct termios orig_termios;
    bool raw_mode;
} state;

int wp_enable_raw_mode() {
    if(state.raw_mode) {
        return 0;
    }

    if(tcgetattr(STDIN_FILENO, &state.orig_termios) == -1) {
        return -1;
    }

    struct termios new_termios = state.orig_termios;
    new_termios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    new_termios.c_oflag &= ~(OPOST);
    new_termios.c_cflag |= (CS8);
    new_termios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios) == -1) {
        return -1;
    }

    state.raw_mode = true;
    return 0;
}

int wp_disable_raw_mode() {
    if(!state.raw_mode) {
        return 0;
    }

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &state.orig_termios) == -1) {
        return -1;
    }

    state.raw_mode = false;
    return 0;
}

int wp_get_screen_size(size_t *rows, size_t *cols) {
    struct winsize ws;

    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        return -1;
    }

    if(ws.ws_col == 0 || ws.ws_row == 0) {
        return -1;
    }

    *rows = (size_t)ws.ws_row;
    *cols = (size_t)ws.ws_col;

    return 0;
}