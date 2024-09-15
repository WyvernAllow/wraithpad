#include "platform.h"
#include <windows.h>
#include <stdbool.h>

struct {
    HANDLE console_handle;
    DWORD orig_mode;
    bool raw_mode;
} state;

int wp_enable_raw_mode() {
    if (state.raw_mode) {
        return 0;
    }

    state.console_handle = GetStdHandle(STD_INPUT_HANDLE);
    if (state.console_handle == INVALID_HANDLE_VALUE) {
        return -1;
    }

    if (!GetConsoleMode(state.console_handle, &state.orig_mode)) {
        return -1;
    }

    DWORD new_mode = state.orig_mode;
    new_mode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
    new_mode |= ENABLE_VIRTUAL_TERMINAL_INPUT;

    if (!SetConsoleMode(state.console_handle, new_mode)) {
        return -1;
    }

    state.raw_mode = true;
    return 0;
}

int wp_disable_raw_mode() {
    if (!state.raw_mode) {
        return 0;
    }

    if (!SetConsoleMode(state.console_handle, state.orig_mode)) {
        return -1;
    }

    state.raw_mode = false;
    return 0;
}

int wp_get_screen_size(size_t *rows, size_t *cols) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    if (!GetConsoleScreenBufferInfo(console_handle, &csbi)) {
        return -1;
    }

    *rows = (size_t)csbi.dwSize.Y;
    *cols = (size_t)csbi.dwSize.X;

    return 0;
}
