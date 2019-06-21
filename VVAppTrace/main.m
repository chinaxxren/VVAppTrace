
#import <UIKit/UIKit.h>
#import "AppDelegate.h"
#import "VVTrace.h"

int main(int argc, char *argv[]) {
    [VVTrace setMinDuration:1];
    [VVTrace startTrace];
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
