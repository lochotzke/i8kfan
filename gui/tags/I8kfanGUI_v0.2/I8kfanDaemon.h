//
//  I8kfanDaemon.h
//  I8kfanGUI
//
//  Created by Andrew Bennett on 5/9/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "I8kfanProtocol.h"

@interface I8kfanDaemon : NSObject <I8kfanProtocol> {
    int baseRpm;
    float lowerThreshold;
    float upperThreshold;
    int maxRpm;
    int currentRpm;
    BOOL fahrenheit;
    BOOL needWrite;
}

- (void)start;

@end
