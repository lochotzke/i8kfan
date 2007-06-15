#import <Cocoa/Cocoa.h>
#import "I8kfanDaemon.h"

int main(int argc, char *argv[])
{
	// ignore restart and stop
    if (argc == 2 && strcmp(argv[1], "start") == 0) {
        // fork off daemon
        if (fork() == 0) {
            NSAutoreleasePool *pool = [NSAutoreleasePool new];
            I8kfanDaemon *daemon = [I8kfanDaemon new];
            NSConnection *connection = [NSConnection defaultConnection];

            // register connection
            [connection setRootObject:daemon];
            [connection registerName:I8kfanDaemonRegisteredName];

            // run runloop
            [daemon start];
            [[NSRunLoop currentRunLoop] run];

            [daemon release];
            [pool release];
        }
    }
	return 0;
}
