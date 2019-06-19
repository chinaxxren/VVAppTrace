//
//  VVTrace.h
//  AppTrace
//
//  Created by Jiangmingz on 2019/6/19.
//  Copyright © 2019 Jiangmingz. All rights reserved.
//

#include <stdio.h>
#include <stdbool.h>


#ifndef VVTrace_h
#define VVTrace_h

bool vv_open(char *ios_path);

void vv_begin_section(const char *name);

void vv_end_section(const char *name);

void vv_sync_wait(void);

#endif /* VVTrace_h */
