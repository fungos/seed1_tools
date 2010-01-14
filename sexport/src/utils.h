#ifndef _UTILS_H_
#define _UTILS_H_

extern bool gVerbose;
extern bool gQuiet;

char *to_upper(const char *str);
const char *get_filename(const char *str);
int create_directory(const char *path);
int file_is_newer_than(const char *src, const char *dst);

#endif
