//
//  VVLog.h
//  AppTrace
//
//  Created by Jiangmingz on 2019/6/19.
//  Copyright © 2019 Jiangmingz. All rights reserved.
//

#ifndef VVLog_h
#define VVLog_h

#include <stdio.h>
#include <stdbool.h>

void log_base_path(char *ios_path);

bool log_open(void);

void log_add_line(const char *line);

#endif /* VVLog_h */
