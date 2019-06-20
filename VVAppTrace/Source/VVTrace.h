
#include <stdio.h>
#include <stdbool.h>


#ifndef VVTrace_h
#define VVTrace_h

bool vv_open(char *ios_path);

void vv_begin_section(const char *name);

void vv_end_section(const char *name);

void vv_sync_wait(void);

#endif /* VVTrace_h */
