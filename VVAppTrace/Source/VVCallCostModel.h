
#import <Foundation/Foundation.h>

@interface VVCallCostModel : NSObject

@property(nonatomic, copy) NSString *className;         //类名
@property(nonatomic, copy) NSString *methodName;        //方法名
@property(nonatomic, assign) BOOL isClassMethod;        //是否是类方法
@property(nonatomic, assign) NSTimeInterval timeCost;   //时间消耗
@property(nonatomic, assign) NSUInteger callDepth;      //Call 层级
@property(nonatomic, copy) NSString *path;              //路径
@property(nonatomic, assign) BOOL lastCall;             //是否是最后一个 Call
@property(nonatomic, assign) BOOL isMain;               //是否是主线程
@property(nonatomic, strong) NSArray <VVCallCostModel *> *subCosts;

@end
