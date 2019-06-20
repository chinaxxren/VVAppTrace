
#ifndef VVFishHook_h
#define VVFishHook_h

#include <stdio.h>

#ifdef __LP64__
/*
 * A structure representing a particular intended rebinding from a symbol
 * name to its replacement
 */
struct rebinding {
    const char *name;
    void *replacement;
    void **replaced;
};

/*
 * For each rebinding in rebindings, rebinds references to external, indirect
 * symbols with the specified name to instead point at replacement for each
 * image in the calling process as well as for all future images that are loaded
 * by the process. If rebind_functions is called more than once, the symbols to
 * rebind are added to the existing list of rebindings, and if a given symbol
 * is rebound more than once, the later rebinding will take precedence.
 */
int fish_rebind_symbols(struct rebinding rebindings[], size_t rebindings_nel);

#endif

#endif /* VVFishHook_h */
