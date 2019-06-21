
#import "ViewController.h"

#import "VVShowLogController.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    self.view.backgroundColor = [UIColor whiteColor];

    self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"显示日志" style:UIBarButtonItemStylePlain target:self action:@selector(show)];
}

- (void)show {
    [self.navigationController pushViewController:[VVShowLogController new] animated:YES];
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    // [self testTrace];

    dispatch_queue_t queue = dispatch_get_global_queue(0, 0);
    dispatch_async(queue, ^{
        [self testTrace];
    });
}

- (void)testTrace {
    sleep(2);
    NSLog(@"Hello World!");
}

@end

