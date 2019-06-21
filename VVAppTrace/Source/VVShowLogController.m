

#import "VVShowLogController.h"
#import "VVShowLogVM.h"
#import "VVTrace.h"

static NSString *CellIdentify = @"CellIdentify";

@interface VVTableViewCell : UITableViewCell

@end

@implementation VVTableViewCell

- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(nullable NSString *)reuseIdentifier {
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        self.textLabel.numberOfLines = 0;
        self.textLabel.font = [UIFont systemFontOfSize:12.0f];
    }

    return self;
}

@end

@interface VVShowLogController () <UITableViewDataSource, UITableViewDelegate>

@property(nonatomic, strong) UITableView *tableView;
@property(nonatomic, strong) VVShowLogVM *showLogVM;

@end

@implementation VVShowLogController

- (void)viewDidLoad {
    [super viewDidLoad];

    [self.view addSubview:self.tableView];

    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        [VVTrace stopTrace];
        [weakSelf.showLogVM loadData];
        [VVTrace resumeTrace];
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf.tableView reloadData];
        });
    });
}

- (void)viewDidLayoutSubviews {
    [super viewDidLayoutSubviews];

    self.tableView.frame = self.view.bounds;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return [self.showLogVM.dataList count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    VVTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentify forIndexPath:indexPath];
    NSString *text = self.showLogVM.dataList[indexPath.row];
    cell.textLabel.text = text;
    return cell;
}

- (VVShowLogVM *)showLogVM {
    if (!_showLogVM) {
        _showLogVM = [VVShowLogVM new];
    }

    return _showLogVM;
}

- (UITableView *)tableView {
    if (!_tableView) {
        _tableView = [[UITableView alloc] initWithFrame:CGRectZero style:UITableViewStylePlain];
        [_tableView registerClass:[VVTableViewCell class] forCellReuseIdentifier:CellIdentify];
        _tableView.rowHeight = 44.0f;
        _tableView.dataSource = self;
        _tableView.delegate = self;
    }

    return _tableView;
}


@end