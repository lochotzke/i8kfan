#import <Cocoa/Cocoa.h>


@interface KeyChain : NSObject {

}
+(NSDictionary *)accessToKeyChain:(NSString *)mode user:(NSString*)user pw:(NSString*)pw;


@end