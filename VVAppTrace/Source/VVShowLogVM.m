//
// Created by Jiangmingz on 2019-06-21.
// Copyright (c) 2019 Jiangmingz. All rights reserved.
//

#import "VVShowLogVM.h"

#import "VVTrace.h"
#import "VVFileReader.h"


@implementation VVShowLogVM

- (NSMutableArray<NSString *> *)dataList {
    if (!_dataList) {
        _dataList = [NSMutableArray<NSString *> new];
    }

    return _dataList;
}

- (void)loadData {
    NSString *traceLogDir = [VVTrace getTraceLogDir];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSDirectoryEnumerator *enumerator = [fileManager enumeratorAtPath:traceLogDir];

    NSString *subPath;
    while ((subPath = [enumerator nextObject])) {
        @autoreleasepool {
            NSString *traceLogPath = [traceLogDir stringByAppendingPathComponent:subPath];
            VVFileReader *reader = [[VVFileReader alloc] initWithFilePath:traceLogPath];
            NSData *lineData = [reader readLineData];
            
            NSDictionary *beginDict;
            while (lineData && [lineData length] > 0) {
                lineData = [lineData subdataWithRange:NSMakeRange(0, lineData.length - 2)];
                NSError *error;
                NSDictionary *dict = [NSJSONSerialization JSONObjectWithData:lineData options:NSJSONReadingAllowFragments error:&error];
                if(!error) {
                    if ([dict[@"ph"] isEqualToString:@"B"]) {
                        beginDict = dict;
                    } else if ([dict[@"ph"] isEqualToString:@"E"]) {
                        NSString *name = dict[@"name"];
                        if (name && [beginDict[@"name"] isEqualToString:name]) {
                            NSString *logString = [NSString stringWithFormat:@"Name:%@ Thread:%@ Duration:%llu", name, dict[@"tid"], [dict[@"ts"] longLongValue] - [beginDict[@"ts"] longLongValue]];
                            [self.dataList addObject:logString];
                        }
                    }
                } else {
                    NSLog(@"%@",error);
                }
        
                lineData = [reader readLineData];
            }
        }
    }
}

@end
