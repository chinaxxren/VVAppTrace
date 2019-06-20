

#ifndef SMCallTraceCore_h
#define SMCallTraceCore_h

#include <stdio.h>
#include <objc/objc.h>

typedef struct {
    __unsafe_unretained Class cls;
    SEL sel;
    uint64_t time; // us (1/1000 ms)
    int depth;
    bool is_main_thread;
} vv_call_record;

extern void vv_call_trace_start(char *ios_path);

extern void vv_call_trace_stop(void);

extern void vv_save_record(bool save);

extern void vv_main_record(bool main);

//default 1000
extern void vv_call_config_min_time(uint64_t us);

//default 3
extern void vv_call_config_max_depth(int depth);

extern vv_call_record *vv_get_call_records(int *num);

extern void vv_clear_call_records(void);


#endif /* SMCallTraceCore_h */
