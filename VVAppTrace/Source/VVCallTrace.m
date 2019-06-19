
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

+ (void)stop {
    vv_call_trace_stop();
}

+ (void)save {
    NSMutableString *mStr = [NSMutableString new];
    NSArray < VVCallTraceTimeCostModel * > *arr = [self loadRecords];
    for (VVCallTraceTimeCostModel *model in arr) {
        //记录方法路径
        model.path = [NSString stringWithFormat:@"[%@ %@]", model.className, model.methodName];
        [self appendRecord:model to:mStr];
    }
    NSLog(@"%@", mStr);
}

+ (void)stopSaveAndClean {
    [VVCallTrace stop];
    [VVCallTrace save];
    vv_clear_call_records();
}

+ (void)appendRecord:(VVCallTraceTimeCostModel *)cost to:(NSMutableString *)mStr {
    [mStr appendFormat:@"%@\n path%@\n", [cost des], cost.path];
    if (cost.subCosts.count < 1) {
        cost.lastCall = YES;
    }

    for (VVCallTraceTimeCostModel *model in cost.subCosts) {
        //记录方法的子方法的路径
        model.path = [NSString stringWithFormat:@"%@ - [%@ %@]", cost.path, model.className, model.methodName];
        [self appendRecord:model to:mStr];
    }
}

+ (NSArray<VVCallTraceTimeCostModel *> *)loadRecords {
    NSMutableArray < VVCallTraceTimeCostModel * > *arr = [NSMutableArray new];
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
        [arr addObject:model];
    }

    NSUInteger count = arr.count;
    for (NSUInteger i = 0; i < count; i++) {
        VVCallTraceTimeCostModel *model = arr[i];
        if (model.callDepth > 0) {
            [arr removeObjectAtIndex:i];
            //Todo:不需要循环，直接设置下一个，然后判断好边界就行
            for (NSUInteger j = i; j < count - 1; j++) {
                //下一个深度小的话就开始将后面的递归的往 sub array 里添加
                if (arr[j].callDepth + 1 == model.callDepth) {
                    NSMutableArray *sub = (NSMutableArray *) arr[j].subCosts;
                    if (!sub) {
                        sub = [NSMutableArray new];
                        arr[j].subCosts = sub;
                    }
                    [sub insertObject:model atIndex:0];
                }
            }
            i--;
            count--;
        }
    }

    return arr;
}


@end
