#include <string>
#include <utility>
#include <functional>
#include <iostream>

#include <Foundation/Foundation.h>
#include <obs.h>

void fetchStringFromUrl(const char *urlString, std::function<void(std::string, int)> callback)
{
    NSString *urlNsString = [[NSString alloc] initWithUTF8String:urlString];
    NSURLSession *session = [NSURLSession sharedSession];
    NSURL *url = [NSURL URLWithString:urlNsString];
    NSURLSessionDataTask *task = [session dataTaskWithURL:url completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
        if (error != NULL) {
            callback("", 1);
        } else if (response == NULL) {
            callback("", 2);
        } else if (data == NULL || data.length == 0) {
            callback("", 3);
        } else if (![response isKindOfClass:NSHTTPURLResponse.class]) {
            callback("", 4);
        } else {
            std::string responseBody((const char *)data.bytes);
            NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse*)response;
            NSInteger code = httpResponse.statusCode;
            callback(responseBody, code < 200 || code > 299 ? 5 : 0);
        }
    }];
    [task resume];
}
