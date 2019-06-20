
#include "VVCallTraceCore.h"

#ifdef __aarch64__
#pragma mark - Record

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <objc/message.h>
#include <objc/runtime.h>
#include <dispatch/dispatch.h>
#include <pthread.h>

#import "VVTrace.h"
#import "VVFishHook.h"

static bool _call_record_enabled = true;
static bool _is_main_thread = true;
static bool _save_record_enabled = false;
static uint64_t _min_time_cost = 1000; //us
static int _max_call_depth = 3;
static pthread_key_t _thread_key;
__unused static id (*orig_objc_msgSend)(id, SEL, ...);

static vv_call_record *_vv_call_records;
//static int otp_record_num;
//static int otp_record_alloc;
static int _vv_record_num;
static int _vv_record_alloc;

typedef struct {
    id self; //通过 object_getClass 能够得到 Class 再通过 NSStringFromClass 能够得到类名
    Class cls;
    SEL cmd; //通过 NSStringFromSelector 方法能够得到方法名
    uint64_t time; //us
    uintptr_t lr; // link register
} thread_call_record;

typedef struct {
    thread_call_record *stack;
    int allocated_length;
    int index;
    bool is_main_thread;
} thread_call_stack;

static inline bool has_prefix(const char *target, char *prefix) {
    uint64_t count = strlen(prefix);
    bool ret = true;
    if (strlen(target) < count)
        return false;
    for (int i = 0; i < count; i++) {
        if (target[i] != prefix[i]) {
            ret = false;
            break;
        }
    }
    
    return ret;
}

static inline thread_call_stack * get_thread_call_stack() {
    thread_call_stack *cs = (thread_call_stack *)pthread_getspecific(_thread_key);
    if (cs == NULL) {
        cs = (thread_call_stack *)malloc(sizeof(thread_call_stack));
        cs->stack = (thread_call_record *)calloc(128, sizeof(thread_call_record));
        cs->allocated_length = 64;
        cs->index = -1;
        cs->is_main_thread = pthread_main_np();
        pthread_setspecific(_thread_key, cs);
    }
    return cs;
}

static void release_thread_call_stack(void *ptr) {
    thread_call_stack *cs = (thread_call_stack *)ptr;
    if (!cs) return;
    if (cs->stack) free(cs->stack);
    free(cs);
}

static inline void push_call_record(id _self, Class _cls, SEL _cmd, uintptr_t lr) {
    thread_call_stack *cs = get_thread_call_stack();
    if (cs) {
        int nextIndex = (++cs->index);
        if (nextIndex >= cs->allocated_length) {
            cs->allocated_length += 64;
            cs->stack = (thread_call_record *)realloc(cs->stack, cs->allocated_length * sizeof(thread_call_record));
        }
        thread_call_record *new_record = &cs->stack[nextIndex];
        new_record->self = _self;
        new_record->cls = _cls;
        new_record->cmd = _cmd;
        new_record->lr = lr;
    
        if (has_prefix(class_getName(_cls), "OS_")) {
            return;
        }
        
        vv_begin_section(sel_getName(new_record->cmd));
        
        if(_call_record_enabled) {
            struct timeval now;
            gettimeofday(&now, NULL);
            new_record->time = (now.tv_sec % 100) * 1000000 + now.tv_usec;
        }
    }
}

static inline uintptr_t pop_call_record() {
    thread_call_stack *cs = get_thread_call_stack();
    int curIndex = cs->index;
    int nextIndex = cs->index--;
    thread_call_record *p_record = &cs->stack[nextIndex];
    if (has_prefix(class_getName(p_record->cls), "OS_")) {
        return p_record->lr;
    }
    
    vv_end_section(sel_getName(p_record->cmd));
    
    if (_call_record_enabled) {
        struct timeval now;
        gettimeofday(&now, NULL);
        uint64_t time = (now.tv_sec % 100) * 1000000 + now.tv_usec;
        if (time < p_record->time) {
            time += 100 * 1000000;
        }
        uint64_t cost = time - p_record->time;
        if (cost > _min_time_cost && cs->index < _max_call_depth) {
            printf("trace->[%s %s] %6.2f\n",class_getName(p_record->cls),sel_getName(p_record->cmd),cost/1000.0f);
            
            if (_save_record_enabled) {
                if (_is_main_thread && !cs->is_main_thread) {
                    return p_record->lr;
                }
                
                if (!_vv_call_records) {
                    _vv_record_alloc = 1024;
                    _vv_call_records = (vv_call_record *)malloc(sizeof(vv_call_record) * _vv_record_alloc);
                }
                _vv_record_num++;
                if (_vv_record_num >= _vv_record_alloc) {
                    _vv_record_alloc += 1024;
                    _vv_call_records = (vv_call_record *)realloc(_vv_call_records, sizeof(vv_call_record) * _vv_record_alloc);
                }

                vv_call_record *call_record = &_vv_call_records[_vv_record_num - 1];
                call_record->cls = p_record->cls;
                call_record->depth = curIndex;
                call_record->sel = p_record->cmd;
                call_record->time = cost;
                call_record->is_main_thread = cs->is_main_thread;
            }
        }
    }
    
    return p_record->lr;
}

void before_objc_msgSend(id self, SEL _cmd, uintptr_t lr) {
    push_call_record(self, object_getClass(self), _cmd, lr);
}

uintptr_t after_objc_msgSend() {
    return pop_call_record();
}

//replacement objc_msgSend (arm64)
// https://blog.nelhage.com/2010/10/amd64-and-va_arg/
// http://infocenter.arm.com/help/topic/com.arm.doc.ihi0055b/IHI0055B_aapcs64.pdf
// https://developer.apple.com/library/ios/documentation/Xcode/Conceptual/iPhoneOSABIReference/Articles/ARM64FunctionCallingConventions.html
#define call(b, value) \
__asm volatile ("stp x8, x9, [sp, #-16]!\n"); \
__asm volatile ("mov x12, %0\n" :: "r"(value)); \
__asm volatile ("ldp x8, x9, [sp], #16\n"); \
__asm volatile (#b " x12\n");

#define save() \
__asm volatile ( \
"stp x8, x9, [sp, #-16]!\n" \
"stp x6, x7, [sp, #-16]!\n" \
"stp x4, x5, [sp, #-16]!\n" \
"stp x2, x3, [sp, #-16]!\n" \
"stp x0, x1, [sp, #-16]!\n");

#define load() \
__asm volatile ( \
"ldp x0, x1, [sp], #16\n" \
"ldp x2, x3, [sp], #16\n" \
"ldp x4, x5, [sp], #16\n" \
"ldp x6, x7, [sp], #16\n" \
"ldp x8, x9, [sp], #16\n" );

#define link(b, value) \
__asm volatile ("stp x8, lr, [sp, #-16]!\n"); \
__asm volatile ("sub sp, sp, #16\n"); \
call(b, value); \
__asm volatile ("add sp, sp, #16\n"); \
__asm volatile ("ldp x8, lr, [sp], #16\n");

#define ret() __asm volatile ("ret\n");

__attribute__((__naked__))
static void hook_Objc_msgSend() {
    // Save parameters.
    save()
    
    __asm volatile ("mov x2, lr\n");
    __asm volatile ("mov x3, x4\n");
    
    // Call our before_objc_msgSend.
    call(blr, &before_objc_msgSend)
    
    // Load parameters.
    load()
    
    // Call through to the original objc_msgSend.
    call(blr, orig_objc_msgSend)
    
    // Save original objc_msgSend return value.
    save()
    
    // Call our after_objc_msgSend.
    call(blr, &after_objc_msgSend)
    
    // restore lr
    __asm volatile ("mov lr, x0\n");
    
    // Load original objc_msgSend return value.
    load()
    
    // return
    ret()
}


#pragma mark public

void vv_call_trace_start(char *ios_path) {
    _call_record_enabled = true;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        pthread_key_create(&_thread_key, &release_thread_call_stack);
        fish_rebind_symbols((struct rebinding[6]){
            {"objc_msgSend", (void *)hook_Objc_msgSend, (void **)&orig_objc_msgSend},
        }, 1);
        vv_open(ios_path);
    });
}

void vv_call_trace_stop(void) {
    _call_record_enabled = false;
}

void vv_save_record(bool save) {
    _save_record_enabled = save;
}

void vv_main_record(bool main) {
    _is_main_thread = main;
}

void vv_call_config_min_time(uint64_t us) {
    _min_time_cost = us;
}
void vv_call_config_max_depth(int depth) {
    _max_call_depth = depth;
}

vv_call_record *vv_get_call_records(int *num) {
    if (num) {
        *num = _vv_record_num;
    }
    return _vv_call_records;
}

void vv_clear_call_records(void) {
    if (_vv_call_records) {
        free(_vv_call_records);
        _vv_call_records = NULL;
    }
    _vv_record_num = 0;
}

#else

void vv_call_trace_start(char *ios_path) {
}

void vv_call_trace_stop(void) {
}

void vv_save_record(bool save) {
}

void vv_main_record(bool main) {
    
}

void vv_call_config_min_time(uint64_t us) {
}

void vv_call_config_max_depth(int depth) {
}

vv_call_record *vv_get_call_records(int *num) {
    if (num) {
        *num = 0;
    }
    return NULL;
}

void vv_clear_call_records(void) {
}

#endif
