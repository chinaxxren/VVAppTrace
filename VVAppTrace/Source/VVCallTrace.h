
#import <Foundation/Foundation.h>
#import "VVCallTraceCore.h"

@class VVCallTraceTimeCostModel;

@interface VVCallTrace : NSObject

// 开始记录
+ (void)start;

+ (void)startWithMaxDepth:(int)depth;

+ (void)startWithMinCost:(double)ms;

+ (void)startWithMaxDepth:(int)depth minCost:(double)ms;

+ (void)isOpenSaveRecords:(BOOL)save;

+ (void)isOnlyMainRecord:(BOOL)main;

// 停止记录
+ (void)stop;

// 保存记录,返回结果用户自己处理
+ (NSArray<VVCallTraceTimeCostModel *> *)getCallRecords;

@end
