#ifndef UTILS_H
#define UTILS_H

/* Reads all the content of `filename` into a null-terminated string. Must be
 * freed by the caller. */
char *slurp_file_str(const char *filename);

#endif /* UTILS_H */
