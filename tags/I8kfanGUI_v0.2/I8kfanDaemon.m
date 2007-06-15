//
//  I8kfanDaemon.m
//  I8kfanGUI
//
//  Created by Andrew Bennett on 5/9/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "I8kfanDaemon.h"
#import "I8kfanProtocol.h"
#import "i8kfan.h"

#define I8kfanApplicationIdentifier     "com.PotatoSalad.I8kfanGUI"

@implementation I8kfanDaemon

- (id)init
{
    if (self = [super init]) {
        // set sane defaults
        lowerThreshold = 50.0;
        upperThreshold = 80.0;
        baseRpm = 1500;
        maxRpm = I8K_FAN_MAX;
    }
    return self;
}

- (void)storePreferences
{
    CFPreferencesSetValue(CFSTR("baseRpm"), (CFPropertyListRef)[NSNumber numberWithInt:baseRpm],
                          CFSTR(I8kfanApplicationIdentifier), kCFPreferencesAnyUser, kCFPreferencesCurrentHost);
    CFPreferencesSetValue(CFSTR("lowerThreshold"), (CFPropertyListRef)[NSNumber numberWithFloat:lowerThreshold],
                          CFSTR(I8kfanApplicationIdentifier), kCFPreferencesAnyUser, kCFPreferencesCurrentHost);
    CFPreferencesSetValue(CFSTR("upperThreshold"), (CFPropertyListRef)[NSNumber numberWithFloat:upperThreshold],
                          CFSTR(I8kfanApplicationIdentifier), kCFPreferencesAnyUser, kCFPreferencesCurrentHost);
    CFPreferencesSetValue(CFSTR("fahrenheit"), (CFPropertyListRef)[NSNumber numberWithBool:fahrenheit],
                          CFSTR(I8kfanApplicationIdentifier), kCFPreferencesAnyUser, kCFPreferencesCurrentHost);
    CFPreferencesSynchronize(CFSTR(I8kfanApplicationIdentifier), kCFPreferencesAnyUser, kCFPreferencesCurrentHost);
}

- (void)readPreferences
{
    CFPropertyListRef property;
    property = CFPreferencesCopyValue(CFSTR("baseRpm"), CFSTR(I8kfanApplicationIdentifier),
               kCFPreferencesAnyUser, kCFPreferencesCurrentHost);
    if (property) {
        baseRpm = [(NSNumber *)property intValue];
    }
    property = CFPreferencesCopyValue(CFSTR("lowerThreshold"), CFSTR(I8kfanApplicationIdentifier),
               kCFPreferencesAnyUser, kCFPreferencesCurrentHost);
    if (property) {
        lowerThreshold = [(NSNumber *)property floatValue];
    }
    property = CFPreferencesCopyValue(CFSTR("upperThreshold"), CFSTR(I8kfanApplicationIdentifier),
               kCFPreferencesAnyUser, kCFPreferencesCurrentHost);
    if (property) {
        upperThreshold = [(NSNumber *)property floatValue];
    }
    property = CFPreferencesCopyValue(CFSTR("fahrenheit"), CFSTR(I8kfanApplicationIdentifier),
               kCFPreferencesAnyUser, kCFPreferencesCurrentHost);
    if (property) {
        fahrenheit = [(NSNumber *)property boolValue];
    }
}

- (void)start
{
    [self readPreferences];
    [NSTimer scheduledTimerWithTimeInterval:5.0 target:self selector:@selector(timer:) userInfo:nil repeats:YES];
}

- (void)timer:(NSTimer *)aTimer
{
    int temp;
    int targetRpm;
    int step;

    temp = i8k_get_cpu_temp();
    
    if (temp < lowerThreshold) {
        targetRpm = baseRpm;
    } else if (temp > upperThreshold) {
        targetRpm = maxRpm;
    } else {
        targetRpm = baseRpm + (temp - lowerThreshold) / (upperThreshold - lowerThreshold) * (maxRpm - baseRpm);
    }

    // adjust fan speed in reasonable steps - no need to be too dynamic
    if (currentRpm == 0) {
        step = targetRpm;
    } else {
        step = (targetRpm - currentRpm) / 6;
        if (abs(step) < 20) {
            step = 0;
        }
    }
    targetRpm = currentRpm = currentRpm + step;
    
    //SMCSetFanRpm(SMC_KEY_FAN0_RPM_MIN, targetRpm);
    //SMCSetFanRpm(SMC_KEY_FAN1_RPM_MIN, targetRpm);

    // save preferences
    if (needWrite) {
        [self storePreferences];
        needWrite = NO;
    }
}

- (int)baseRpm
{
	return baseRpm;
}

- (void)setBaseRpm:(int)newBaseRpm
{
	baseRpm = newBaseRpm;
    needWrite = YES;
}

- (float)lowerThreshold
{
	return lowerThreshold;
}

- (void)setLowerThreshold:(float)newLowerThreshold
{
	lowerThreshold = newLowerThreshold;
    needWrite = YES;
}

- (float)upperThreshold
{
	return upperThreshold;
}

- (void)setUpperThreshold:(float)newUpperThreshold
{
	upperThreshold = newUpperThreshold;
    needWrite = YES;
}

- (BOOL)fahrenheit
{
	return fahrenheit;
}

- (void)setFahrenheit:(BOOL)newFahrenheit
{
	fahrenheit = newFahrenheit;
    needWrite = YES;
}

- (void)temperature:(float *)temperature leftFanRpm:(int *)leftFanRpm rightFanRpm:(int *)rightFanRpm
{
    if (temperature) {
        *temperature = i8k_get_cpu_temp();
    }
    if (leftFanRpm) {
        *leftFanRpm = i8k_get_fan_speed(I8K_FAN_PRIMARY);
    }
    if (rightFanRpm) {
        *rightFanRpm = i8k_get_fan_speed(I8K_FAN_SECONDARY);
    }
}

@end
