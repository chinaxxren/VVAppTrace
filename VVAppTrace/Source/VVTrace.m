
#import "VVTrace.h"

#include "VVTraceImpl.h"

@implementation VVTrace

+ (void)startTrace {
    NSString *rootDir = [NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES) firstObject];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *workDir = [rootDir stringByAppendingPathComponent:@"apptrace"];

    if ([fileManager fileExistsAtPath:workDir]) {
        [fileManager removeItemAtPath:workDir error:nil];
    }

    [fileManager createDirectoryAtPath:workDir withIntermediateDirectories:YES attributes:nil error:nil];

    
    vv_start([workDir UTF8String]);
}

+ (void)endTrace {
    vv_stop_print();
}

+ (void)setMinDuration:(int)minDuration {
    vv_min_duration(minDuration * 1000);
}

@end
