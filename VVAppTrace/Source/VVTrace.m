
#import "VVTrace.h"

#include "VVTraceImpl.h"

@implementation VVTrace

+ (void)startTrace {
    NSString *workDir = [self getTraceLogDir];

    NSFileManager *fileManager = [NSFileManager defaultManager];
    if ([fileManager fileExistsAtPath:workDir]) {
        [fileManager removeItemAtPath:workDir error:nil];
    }
    [fileManager createDirectoryAtPath:workDir withIntermediateDirectories:YES attributes:nil error:nil];

    vv_start([workDir UTF8String]);
}

+ (NSString *)getTraceLogDir {
    NSString *rootDir = [NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES) firstObject];
    return [rootDir stringByAppendingPathComponent:@"apptrace"];
}

+ (void)resumeTrace {
    vv_resume_print();
}

+ (void)stopTrace {
    vv_stop_print();
}

+ (void)setMinDuration:(int)minDuration {
    vv_min_duration(minDuration * 1000);
}

@end
