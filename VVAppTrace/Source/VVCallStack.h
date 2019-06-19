
#import <Foundation/Foundation.h>
#import "VVCallLib.h"

typedef NS_ENUM(NSUInteger, VVCallStackType) {
    VVCallStackTypeAll,     //全部线程
    VVCallStackTypeMain,    //主线程
    VVCallStackTypeCurrent  //当前线程
};

@interface VVCallStack : NSObject

+ (NSString *)call_stack_with_type:(VVCallStackType)type;

extern NSString *vv_stack_of_thread(thread_t thread);

@end
