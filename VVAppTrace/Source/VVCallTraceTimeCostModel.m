

#import "VVCallTraceTimeCostModel.h"

@implementation VVCallTraceTimeCostModel

- (NSString *)des {
    NSMutableString *str = [NSMutableString new];
    [str appendFormat:@"%2d| ", (int) _callDepth];
    [str appendFormat:@"%6.2f|", _timeCost];
    for (NSUInteger i = 0; i < _callDepth; i++) {
        [str appendString:@"  "];
    }
    [str appendFormat:@"%s[%@ %@]", (_isClassMethod ? "+" : "-"), _className, _methodName];
    return str;
}

@end
