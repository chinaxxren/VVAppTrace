
#import <Foundation/Foundation.h>

//http://stackoverflow.com/questions/3707427/how-to-read-data-from-nsfilehandle-line-by-line/3711079#3711079

@interface VVFileReader : NSObject {
    NSString *filePath;
    NSFileHandle *fileHandle;
    unsigned long long currentOffset;
    unsigned long long totalFileLength;

    NSString *lineDelimiter;
    NSUInteger chunkSize;
}

@property(nonatomic, copy) NSString *lineDelimiter;
@property(nonatomic,assign) NSUInteger chunkSize;

- (id)initWithFilePath:(NSString *)path;

- (NSData *)readLineData;

- (NSString *)readLine;

- (NSString *)readTrimmedLine;

@end
