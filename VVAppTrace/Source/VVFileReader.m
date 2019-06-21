
#import "VVFileReader.h"

@interface NSData (VVExtend)

- (NSRange)vv_rangeOfData:(NSData *)dataToFind;

@end

@implementation NSData (VVExtend)

- (NSRange)vv_rangeOfData:(NSData *)dataToFind {

    const void *bytes = [self bytes];
    NSUInteger length = [self length];

    const void *searchBytes = [dataToFind bytes];
    NSUInteger searchLength = [dataToFind length];
    NSUInteger searchIndex = 0;

    NSRange foundRange = {NSNotFound, searchLength};
    for (NSUInteger index = 0; index < length; index++) {
        if (((char *) bytes)[index] == ((char *) searchBytes)[searchIndex]) {
            //the current character matches
            if (foundRange.location == NSNotFound) {
                foundRange.location = index;
            }
            searchIndex++;
            if (searchIndex >= searchLength) {
                return foundRange;
            }
        } else {
            searchIndex = 0;
            foundRange.location = NSNotFound;
        }
    }

    return foundRange;
}

@end

@implementation VVFileReader

- (id)initWithFilePath:(NSString *)path {
    if ((self = [super init])) {
        fileHandle = [NSFileHandle fileHandleForReadingAtPath:path];
        if (fileHandle == nil) {
            return nil;
        }

        lineDelimiter = @"\n";
        filePath = path;
        currentOffset = 0ULL;
        chunkSize = 10;
        [fileHandle seekToEndOfFile];
        totalFileLength = [fileHandle offsetInFile];
        //we don't need to seek back, since readLine will do that.
    }
    return self;
}

- (void)dealloc {
    [fileHandle closeFile];
    fileHandle = nil;
    filePath = nil;
    lineDelimiter = nil;
    currentOffset = 0ULL;
}

- (NSData *)readLineData {
    if (currentOffset >= totalFileLength) {
        return nil;
    }

    NSData *newLineData = [lineDelimiter dataUsingEncoding:NSUTF8StringEncoding];
    [fileHandle seekToFileOffset:currentOffset];
    NSMutableData *currentData = [[NSMutableData alloc] init];
    BOOL shouldReadMore = YES;

    @autoreleasepool {
        while (shouldReadMore) {
            if (currentOffset >= totalFileLength) {
                break;
            }

            NSData *chunk = [fileHandle readDataOfLength:chunkSize];
            if (chunk.length > 1) {
                char *bytes = (char *)chunk.bytes;
                if (!bytes[0]) {
                    break;
                }
            } else {
                break;
            }

            NSRange newLineRange = [chunk vv_rangeOfData:newLineData];
            if (newLineRange.location != NSNotFound) {

                //include the length so we can include the delimiter in the string
                chunk = [chunk subdataWithRange:NSMakeRange(0, newLineRange.location + [newLineData length])];
                shouldReadMore = NO;
            }
            [currentData appendData:chunk];
            currentOffset += [chunk length];
        }
    }

    return currentData;
}

- (NSString *)readLine {
    NSData *currentData = [self readLineData];
    if (!currentData) {
        return nil;
    }

    NSString *line = [[NSString alloc] initWithData:currentData encoding:NSUTF8StringEncoding];
    return line;
}

- (NSString *)readTrimmedLine {
    return [[self readLine] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
}

@end
