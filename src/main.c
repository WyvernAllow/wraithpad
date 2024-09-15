#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "platform.h"
#include "keys.h"

struct wp_row {
    size_t size;
    char *chars;
};

struct wp_state {
    bool is_running;

    size_t cursor_x;
    size_t cursor_y;

    size_t screen_cols;
    size_t screen_rows;

    size_t num_rows;
    struct wp_row row;
};

int wp_state_init(struct wp_state *state) {
    state->is_running = true;
    state->cursor_x = 0;
    state->cursor_y = 0;

    if(wp_get_screen_size(&state->screen_rows, &state->screen_cols) == -1) {
        fprintf(stderr, "Failed to get screen size");
        return -1;
    }

    state->num_rows = 0;

    return 0;
}

int wp_open(struct wp_state *state, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if(!fp) {
        fprintf(stderr, "Failed to open file. %s\n", filename);
        return -1;
    }

    char *line = NULL;
    size_t line_cap = 0;
    size_t line_len;

    line_len = getline(&line, &line_cap, fp);
    if(line_len != -1) {
        while (line_len > 0 && (line[line_len - 1] == '\n' || line[line_len - 1] == '\r')) {
            line_len--;
        }

        state->row.size = line_len;
        state->row.chars = malloc(line_len + 1);
        memcpy(state->row.chars, line, line_len);
        state->row.chars[line_len] = '\0';
        state->num_rows = 1;
    }

    free(line);
    fclose(fp);

    return 0;
}

void wp_draw_welcome_message(struct wp_state *state) {
    char *welcome_message = "wraithpad editor -- version 0.1.0";
    int welcome_len = strlen(welcome_message);

    int padding = (state->screen_cols - welcome_len) / 2;
    if (padding > 0) {
        fputs("~", stdout);
        padding--;
    }

    while (padding--) {
        fputs(" ", stdout);
    }

    fputs(welcome_message, stdout);
}

void wp_draw_row(struct wp_state *state, size_t y) {
    if (y >= state->num_rows) {
        fputs("~", stdout);
    } else {
        size_t len = state->row.size;
        if (len > state->screen_cols) {
            len = state->screen_cols;
        }
        fwrite(state->row.chars, sizeof(char), len, stdout);
    }
}

void wp_draw(struct wp_state *state) {
    fputs("\x1b[?25l", stdout);
    fputs("\x1b[H", stdout);

    for(size_t y = 0; y < state->screen_rows; y++) {
        if (state->num_rows == 0 && y == state->screen_rows / 2) {
            wp_draw_welcome_message(state);
        } else {
            wp_draw_row(state, y);
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

void wp_update(struct wp_state *state) {
    struct wp_key key = wp_read_key();

    switch (key.type) {
    case WP_KEY_ARROW_UP:
        if(state->cursor_y != 0) {
            state->cursor_y--;
        }
        break;
    case WP_KEY_ARROW_DOWN:
        if(state->cursor_y != state->screen_rows - 1) {
            state->cursor_y++;
        }
        break;
    case WP_KEY_ARROW_LEFT:
        if(state->cursor_x != 0) {
            state->cursor_x--;
        }
        break;
    case WP_KEY_ARROW_RIGHT:
        if(state->cursor_x != state->screen_cols - 1) {
            state->cursor_x++;
        }
        break;
    case WP_KEY_CHAR:
        if(wp_ctrl_key(key, 'q')) {
            state->is_running = false;
        }
        break;
    }
}

int main(int argc, char **argv) {
    if(wp_enable_raw_mode() == -1) {
        fputs("\x1b[2J", stdout);
        fprintf(stderr, "Failed to enable raw mode");
        wp_disable_raw_mode();
        return EXIT_FAILURE;
    }

    struct wp_state state;
    wp_state_init(&state);

    if (argc >= 2) {
        if(wp_open(&state, argv[1]) == -1) {
            wp_disable_raw_mode();
            return EXIT_FAILURE;
        }
    }

    while(state.is_running) {
        wp_draw(&state);
        wp_update(&state);
    }

    wp_disable_raw_mode();
    fputs("\x1b[2J", stdout);
    return EXIT_SUCCESS;
}