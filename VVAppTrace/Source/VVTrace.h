
#import <Foundation/Foundation.h>

@interface VVTrace : NSObject

// 开始检测
+ (void)startTrace;

+ (void)resumeTrace;

// 暂停检测，不再写入检测数据
+ (void)stopTrace;

// 设置打印最小方法执行时间 单位毫秒 默认为1毫秒
+ (void)setMinDuration:(int)minDuration;

+ (NSString *)getTraceLogDir;

@end
