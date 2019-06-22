//
// Created by Jiangmingz on 2019-06-21.
// Copyright (c) 2019 Jiangmingz. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface VVShowLogVM : NSObject

@property(nonatomic, strong) NSMutableArray<NSString *> *dataList;

- (void)loadData;

@end