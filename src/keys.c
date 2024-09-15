#include "keys.h"
#include <stdint.h>
#include <stdio.h>

#define WP_CTRL_KEY(k) ((k) & 0x1F)

static struct wp_key wp_make_key(enum wp_key_type type) {
    struct wp_key key = {
        .type = type,
        .character = '\0',
    };

    return key;
}

static struct wp_key wp_make_char(char c) {
    bool is_ctrl = c == WP_CTRL_KEY(c);

    struct wp_key key = {
        .type = WP_KEY_CHAR,
        .character = c
    };

    return key;
}

bool wp_ctrl_key(struct wp_key key, char c) {
    if(key.type != WP_KEY_CHAR) {
        return false;
    }
    
    return key.character == WP_CTRL_KEY(c);
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