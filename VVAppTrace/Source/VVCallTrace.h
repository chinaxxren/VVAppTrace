
#import <Foundation/Foundation.h>
#import "VVCallTraceCore.h"

@class VVCallCostModel;

@interface VVCallTrace : NSObject

// 开始记录
+ (void)start;

+ (void)startWithMaxDepth:(int)depth;

+ (void)startWithMinCost:(double)ms;

+ (void)startWithMaxDepth:(int)depth minCost:(double)ms;

+ (void)isOnlyMainRecord:(BOOL)isMain;

+ (void)isOpenSaveRecords:(BOOL)isSave;

// 停止记录
+ (void)stop;

// 返回记录用户自己处理
+ (NSArray<VVCallCostModel *> *)getSaveRecords;

@end
