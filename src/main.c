#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "platform.h"

#define WP_CTRL_KEY(k) ((k) & 0x1F)

struct wp_state {
    bool is_running;

    size_t cursor_x;
    size_t cursor_y;

    size_t screen_cols;
    size_t screen_rows;
};

int wp_state_init(struct wp_state *state) {
    state->is_running = true;
    state->cursor_x = 0;
    state->cursor_y = 0;

    if(wp_get_screen_size(&state->screen_rows, &state->screen_cols) == -1) {
        fprintf(stderr, "Failed to get screen size");
        return -1;
    }

    return 0;
}

void wp_draw(struct wp_state *state) {
    fputs("\x1b[?25l", stdout);
    fputs("\x1b[H", stdout);

    for(int y = 0; y < state->screen_rows; y++) {
        if (y == state->screen_rows / 2) {
            char *welcome_message = "wraithpad editor -- version 0.1.0";
            int welcome_len = strlen(welcome_message);

            int padding = (state->screen_cols - welcome_len) / 2;
            if (padding) {
                fputs("~", stdout);
                padding--;
            }

            while (padding--) {
                fputs(" ", stdout);
            }

            fputs(welcome_message, stdout);
        } else {
            fputs("~", stdout);
        }

        fputs("\x1b[K", stdout);

        if (y < state->screen_rows - 1) {
            fputs("\r\n", stdout);
        }
    }

    fprintf(stdout, "\x1b[%zu;%zuH", state->cursor_y + 1, state->cursor_x + 1);

    fputs("\x1b[?25h", stdout);
    fflush(stdout);
}

enum wp_key_type {
    WP_KEY_ARROW_UP,
    WP_KEY_ARROW_DOWN,
    WP_KEY_ARROW_LEFT,
    WP_KEY_ARROW_RIGHT,

    WP_KEY_ARROW_CHAR,
};

struct wp_key {
    enum wp_key_type type;
    char character;
};

struct wp_key wp_make_key(enum wp_key_type type) {
    struct wp_key key = {
        .type = type,
        .character = '\0'
    };

    return key;
}

struct wp_key wp_make_char(char c) {
    struct wp_key key = {
        .type = WP_KEY_ARROW_CHAR,
        .character = c
    };

    return key;
}

struct wp_key wp_read_key() {
    char c;
    fread(&c, sizeof(c), 1, stdin);

    if(c == '\x1b') {
        char seq[3];

        if(fread(&seq[0], sizeof(char), 1, stdin) != 1) {
            return wp_make_char('\x1b');
        }

        if(fread(&seq[1], sizeof(char), 1, stdin) != 1) {
            return wp_make_char('\x1b');
        }

        if(seq[0] == '[') {
            switch (seq[1]) {
                case 'A': return wp_make_key(WP_KEY_ARROW_UP);
                case 'B': return wp_make_key(WP_KEY_ARROW_DOWN);
                case 'C': return wp_make_key(WP_KEY_ARROW_RIGHT);
                case 'D': return wp_make_key(WP_KEY_ARROW_LEFT);
            }
        }

        return wp_make_char('\x1b');
    }

    return wp_make_char(c);
}

void wp_update(struct wp_state *state) {
    struct wp_key key = wp_read_key();

    switch (key.type) {
    case WP_KEY_ARROW_UP:
        state->cursor_y--;
        break;
    case WP_KEY_ARROW_DOWN:
        state->cursor_y++;
        break;
    case WP_KEY_ARROW_LEFT:
        state->cursor_x--;
        break;
    case WP_KEY_ARROW_RIGHT:
        state->cursor_x++;
        break;
    case WP_KEY_ARROW_CHAR:
        if(key.character == WP_CTRL_KEY('q')) {
            state->is_running = false;
        }
      break;
    }
}

int main(int argc, char **argv) {
    if(wp_enable_raw_mode() == -1) {
        fputs("\x1b[2J", stdout);
        fprintf(stderr, "Failed to enable raw mode");
        return EXIT_FAILURE;
    }

    struct wp_state state;
    wp_state_init(&state);

    while(state.is_running) {
        wp_draw(&state);
        wp_update(&state);
    }

    if(wp_disable_raw_mode() == -1) {
        fputs("\x1b[2J", stdout);
        fprintf(stderr, "Failed to disable raw mode");
        return EXIT_FAILURE;
    }

    fputs("\x1b[2J", stdout);
    return EXIT_SUCCESS;
}