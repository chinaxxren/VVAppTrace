
#ifndef VVLog_h
#define VVLog_h

#include <stdio.h>
#include <stdbool.h>

void log_base_path(char *ios_path);

bool log_open(void);

void log_add_line(const char *line);

#endif /* VVLog_h */
