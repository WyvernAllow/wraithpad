#ifndef WP_KEYS_H
#define WP_KEYS_H
#include <stdbool.h>

enum wp_key_type {
    WP_KEY_ARROW_UP,
    WP_KEY_ARROW_DOWN,
    WP_KEY_ARROW_LEFT,
    WP_KEY_ARROW_RIGHT,

    WP_KEY_CHAR,
};

struct wp_key {
    enum wp_key_type type;
    char character;
};

bool wp_ctrl_key(struct wp_key key, char c);
struct wp_key wp_read_key();


#endif // WP_KEYS_H