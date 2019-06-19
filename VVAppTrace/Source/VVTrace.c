//
//  VVTrace.c
//  AppTrace
//
//  Created by Jiangmingz on 2019/6/19.
//  Copyright © 2019 Jiangmingz. All rights reserved.
//

#include "VVTrace.h"

#include <stdint.h>
#include <stdbool.h>
#include <objc/message.h>
#include <objc/runtime.h>
#include <dispatch/dispatch.h>
#include <mach/mach_time.h>
#include <pthread/pthread.h>
#include <string.h>

#import "VVLog.h"

dispatch_queue_t queue_t;
uint64_t begin_time;
mach_timebase_info_data_t time_info;
__uint64_t main_thread_id = 0;

bool vv_open(char *ios_path) {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        log_base_path(ios_path);
        log_open();

        queue_t = dispatch_queue_create("trace.queue", DISPATCH_QUEUE_SERIAL);
        mach_timebase_info(&time_info);
        begin_time = mach_absolute_time() * time_info.numer / time_info.denom;
    });

    return true;
}

void write_section(const char *name, const char *ph) {
    pthread_t thread = pthread_self();
    __uint64_t thread_id = 0;
    pthread_threadid_np(thread, &thread_id);

    uint64_t time = mach_absolute_time() * time_info.numer / time_info.denom;
    uint64_t elapsed = (time - begin_time) / 1000;

    if (main_thread_id == 0 && pthread_main_np() != 0) {
        main_thread_id = thread_id;
    }

    if (main_thread_id == thread_id) {
        thread_id = 0; // just make main thread id zero
    }

    dispatch_async(queue_t, ^{
        char line_json[1024] = {};
        sprintf(line_json, "{\"name\":\"%s\",\"cat\":\"catname\",\"ph\":\"%s\",\"pid\":666,\"tid\":%llu,\"ts\":%llu}", name, ph, thread_id, elapsed);
        log_add_line(line_json);
    });
}

void vv_begin_section(const char *name) {
    write_section(name, "B");
}

void vv_end_section(const char *name) {
    write_section(name, "E");
}

void vv_sync_wait(void) {
    dispatch_sync(queue_t, ^{
        printf("SyncWait");
    });
}
