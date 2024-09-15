#ifndef WP_PLATFORM_H
#define WP_PLATFORM_H
#include <stdint.h>
#include <stddef.h>

int wp_enable_raw_mode();
int wp_disable_raw_mode();

int wp_get_screen_size(size_t *rows, size_t *cols);

#endif // WP_PLATFORM_H