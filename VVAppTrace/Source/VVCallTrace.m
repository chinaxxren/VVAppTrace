
#import "VVCallTrace.h"

#import "VVCallLib.h"
#import "VVCallTraceTimeCostModel.h"
#import "VVCallTraceCore.h"

@implementation VVCallTrace

#pragma mark - Trace
#pragma mark - OC Interface

+ (void)start {
    NSString *rootDir = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES)[0];
    rootDir = [rootDir stringByAppendingPathComponent:@"appletrace"];

    NSFileManager *defaultManager = [NSFileManager defaultManager];
    if ([defaultManager fileExistsAtPath:rootDir]) {
        [defaultManager removeItemAtPath:rootDir error:NULL];
    }

    [defaultManager createDirectoryAtPath:rootDir withIntermediateDirectories:YES attributes:nil error:nil];

    vv_call_trace_start((char *) rootDir.UTF8String);
}

+ (void)startWithMaxDepth:(int)depth {
    vv_call_config_max_depth(depth);
    [VVCallTrace start];
}

+ (void)startWithMinCost:(double)ms {
    vv_call_config_min_time(ms * 1000);
    [VVCallTrace start];
}

+ (void)startWithMaxDepth:(int)depth minCost:(double)ms {
    vv_call_config_max_depth(depth);
    vv_call_config_min_time(ms * 1000);
    [VVCallTrace start];
}

+ (void)isOpenSaveRecords:(BOOL)save {
    vv_save_record(save ? true : false);
}

+ (void)isOnlyMainRecord:(BOOL)main {
    vv_main_record(main ? true:false);
}

+ (void)stop {
    vv_call_trace_stop();
    vv_clear_call_records();
}

+ (NSArray<VVCallTraceTimeCostModel *> *)getCallRecords {
    NSMutableArray < VVCallTraceTimeCostModel * > *array = [NSMutableArray new];
    int num = 0;
    vv_call_record *records = vv_get_call_records(&num);
    for (int i = 0; i < num; i++) {
        vv_call_record *rd = &records[i];
        VVCallTraceTimeCostModel *model = [VVCallTraceTimeCostModel new];
        model.className = NSStringFromClass(rd->cls);
        model.methodName = NSStringFromSelector(rd->sel);
        model.isClassMethod = class_isMetaClass(rd->cls);
        model.timeCost = (double) rd->time / 1000000.0;
        model.callDepth = (NSUInteger) rd->depth;
        model.isMain = rd->is_main_thread;
        [array addObject:model];
    }

    NSUInteger count = array.count;
    for (NSUInteger i = 0; i < count; i++) {
        VVCallTraceTimeCostModel *model = array[i];
        if (model.callDepth > 0) {
            [array removeObjectAtIndex:i];
            //Todo:不需要循环，直接设置下一个，然后判断好边界就行
            for (NSUInteger j = i; j < count - 1; j++) {
                //下一个深度小的话就开始将后面的递归的往 sub array 里添加
                if (array[j].callDepth + 1 == model.callDepth) {
                    NSMutableArray *sub = (NSMutableArray *) array[j].subCosts;
                    if (!sub) {
                        sub = [NSMutableArray new];
                        array[j].subCosts = sub;
                    }
                    [sub insertObject:model atIndex:0];
                }
            }
            i--;
            count--;
        }
    }

    return array;
}


@end
