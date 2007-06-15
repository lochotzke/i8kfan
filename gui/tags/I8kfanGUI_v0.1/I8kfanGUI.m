//
//  I8kfanGUI.m
//  I8kfanGUI
//
//  Created by Andrew Bennett on 5/9/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "I8kfanGUI.h"
#include "i8kfan.h"

@implementation I8kfanGUI

- (void)dealloc
{
    [statusItem release];
    [timer release];
    [super dealloc];
}

- (void)updateOutput:(NSTimer *)aTimer
{
    int temp = i8k_get_cpu_temp();
    unichar c = 0x00b0;
    NSString *degreeChar = [NSString stringWithCharacters:&c length:1];
    [temperatureField setStringValue:[NSString stringWithFormat:@"%d %@C", temp, degreeChar]];
    [self setPeakCPUTemp:temp];
    [peakField setStringValue:[NSString stringWithFormat:@"%d %@C", peakCPUTemp, degreeChar]];  
    [statusItem setTitle:[NSString stringWithFormat:@"%d", temp]]; 
    
    [primaryFanSpeedField setStringValue:[NSString stringWithFormat:@"%d RPM", i8k_get_fan_speed(I8K_FAN_PRIMARY)]];
    int primary_fan_status = i8k_get_fan_status(I8K_FAN_PRIMARY);
    NSString *primaryFanStatus;
    if (primary_fan_status == I8K_FAN_OFF) {
        primaryFanStatus = @"Off";
        [primaryFanSegControl setSelectedSegment:0];
    } else if (primary_fan_status == I8K_FAN_LOW) {
        primaryFanStatus = @"Low";
        [primaryFanSegControl setSelectedSegment:1];
    } else if (primary_fan_status == I8K_FAN_HIGH) {
        primaryFanStatus = @"High";
        [primaryFanSegControl setSelectedSegment:2];
    } else {
        primaryFanStatus = [NSString stringWithFormat:@"Nil (%d)", primary_fan_status];
    }
    [primaryFanStatusField setStringValue:primaryFanStatus];
    
    [self setPrimaryFanSpeedWithTemp:temp status:primary_fan_status];
}

- (void)awakeFromNib
{
    // connect to object controller
    [fileOwnerController setContent:self];
    
    // set default values
    peakCPUTemp = 0;
    [versionField setStringValue:@"version 0.1"];
    
    NSMenuItem *menuItem;
    statusMenu = [[NSMenu alloc] initWithTitle:@"Apps"];
    menuItem = [statusMenu addItemWithTitle:@"Status" action:@selector(showStatusPanel:) keyEquivalent:@""];
    [menuItem setTarget:self];
    [statusMenu addItem:[NSMenuItem separatorItem]];
    unichar c = 0x2026;
    NSString *ellipsisChar = [NSString stringWithCharacters:&c length:1];
    menuItem = [statusMenu addItemWithTitle:[NSString stringWithFormat:@"Preferences%@", ellipsisChar] action:@selector(showPreferencesPanel:) keyEquivalent:@","];
    [menuItem setTarget:self];
    menuItem = [statusMenu addItemWithTitle:@"About I8kfanGUI" action:@selector(showAboutPanel:) keyEquivalent:@""];
    [menuItem setTarget:self];
    [statusMenu addItem:[NSMenuItem separatorItem]];
    menuItem = [statusMenu addItemWithTitle:@"Quit I8kfanGUI" action:@selector(terminate:) keyEquivalent:@"q"];
    [menuItem setTarget:NSApp];
    statusItem = [[[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength] retain];
    [statusItem setHighlightMode:YES];
    [statusItem setMenu:statusMenu];
    [statusItem setTitle:[NSString stringWithString:@"0"]];
    [statusItem setImage: [NSImage imageNamed: @"I8kfanGUI_system"]];
    [statusItem setEnabled:YES];
    unichar c1 = 0x00b0;
    NSString *degreeChar = [NSString stringWithCharacters:&c1 length:1];
    [statusItem setToolTip:[NSString stringWithFormat:@"%@C", degreeChar]];
    [statusItem setAction:@selector(updateOutput:)];
    [statusItem setTarget:self];
    
    /*NSMenuItem *menuItem;
    appsMenu = [[NSMenu alloc] initWithTitle: @"Apps"];
    unichar c = 0x2026;
    NSString *ellipsisChar = [NSString stringWithCharacters:&c length:1];
    menuItem = [appsMenu addItemWithTitle: [NSString stringWithFormat: @"Preferences%@", ellipsisChar] action: @selector (showSettingsPanel:) keyEquivalent: @","];
    [menuItem setTarget: self];
    menuItem = [appsMenu addItemWithTitle: @"Refresh" action: @selector (terminate:) keyEquivalent: @""];
    [menuItem setTarget: self];
    [appsMenu addItem: [NSMenuItem separatorItem]];
    menuItem = [appsMenu addItemWithTitle: @"Quit TigerLaunch" action: @selector (terminate:) keyEquivalent: @""];
    [menuItem setTarget: NSApp];
    appsStatusItem = [[NSStatusBar systemStatusBar] statusItemWithLength: NSVariableStatusItemLength];
    [appsStatusItem retain];
    //[appsStatusItem setTitle:@"0"];
    [appsStatusItem setImage: [NSImage imageNamed: @"i8kfan-24"]];//@"I8kfanGUI_system"]];
    [appsStatusItem setMenu: appsMenu];
    [appsStatusItem setHighlightMode: YES];*/
    
    // update output immediatly, then every 2 seconds
    [self updateOutput:nil];
    timer = [NSTimer scheduledTimerWithTimeInterval:2 target:self selector:@selector(updateOutput:) userInfo:nil repeats:YES];
}

- (void)setPeakCPUTemp: (int)temp
{
    if (peakCPUTemp < temp) {
        peakCPUTemp = temp;
    }
}

- (int)setPrimaryFanSpeedWithTemp: (int)temp status: (int)status
{
    if ([tempCheckBox state] == 1) {
        int state1 = [[thresholdList1 selectedItem] tag];
        int state2 = [[thresholdList2 selectedItem] tag];
        int state3 = [[thresholdList3 selectedItem] tag];
        int state4 = [[thresholdList4 selectedItem] tag];
        
        int on1  = [[onThresholdField1 stringValue] intValue];
        int off1 = [[offThresholdField1 stringValue] intValue];
        int on2  = [[onThresholdField2 stringValue] intValue];
        int off2 = [[offThresholdField2 stringValue] intValue];
        int on3  = [[onThresholdField3 stringValue] intValue];
        int off3 = [[offThresholdField3 stringValue] intValue];
        int on4  = [[onThresholdField4 stringValue] intValue];
        int off4 = [[offThresholdField4 stringValue] intValue];
        
        int desired_status = status;
        
        if (temp <= off1)
            desired_status = (int)[self getSpeedFromState:0];
        if (temp >= on1 && temp <= off2)
            desired_status = (int)[self getSpeedFromState:state1];
        if (temp >= on2 && temp <= off3)
            desired_status = (int)[self getSpeedFromState:state2];
        if (temp >= on3 && temp <= off4)
            desired_status = (int)[self getSpeedFromState:state3];
        if (temp >= on4)
            desired_status = (int)[self getSpeedFromState:state4];
        
        if ([overdriveCheckBox state] == 1) {
            i8k_set_fan(I8K_FAN_PRIMARY, desired_status);
        } else {
            if (status != desired_status)
                i8k_set_fan(I8K_FAN_PRIMARY, desired_status);
        }
        return desired_status;
    }
    return -1;
}

- (int)getSpeedFromState: (int)state
{
    if (state == 2) {
        return I8K_FAN_HIGH;
    } else if (state == 1) {
        return I8K_FAN_LOW;
    } else {
        return I8K_FAN_OFF;
    }
}

- (void)terminate:(id)sender
{
	[timer invalidate];
	[[NSApplication sharedApplication] terminate:self];
}

-(void)setPrimaryFanOff:(id)sender
{
    i8k_set_fan(I8K_FAN_PRIMARY, I8K_FAN_OFF);
}

-(void)setPrimaryFanLow:(id)sender
{
    i8k_set_fan(I8K_FAN_PRIMARY, I8K_FAN_LOW);
}

-(void)setPrimaryFanHigh:(id)sender
{
    i8k_set_fan(I8K_FAN_PRIMARY, I8K_FAN_HIGH);
}

-(void)setSecondaryFanOff:(id)sender
{
    i8k_set_fan(I8K_FAN_SECONDARY, I8K_FAN_OFF);
}

-(void)setSecondaryFanLow:(id)sender
{
    i8k_set_fan(I8K_FAN_SECONDARY, I8K_FAN_LOW);
}

-(void)setSecondaryFanHigh:(id)sender
{
    i8k_set_fan(I8K_FAN_SECONDARY, I8K_FAN_HIGH);
}

-(IBAction)primaryFanSegControlClicked:(id)sender
{
    int clickedSegment = [sender selectedSegment];
    if (clickedSegment == 0) {
        [self setPrimaryFanOff:sender];
    } else if (clickedSegment == 1) {
        [self setPrimaryFanLow:sender];
    } else if (clickedSegment == 2) {
        [self setPrimaryFanHigh:sender];
    }
}

- (IBAction)showStatusPanel:(id)sender
{
    [NSApp unhide];
    [mainTabView selectTabViewItem:[mainTabView tabViewItemAtIndex:0]];
}

- (IBAction)showPreferencesPanel:(id)sender
{
    [NSApp unhide];
    [mainTabView selectTabViewItem:[mainTabView tabViewItemAtIndex:1]];
}

- (IBAction)showAboutPanel:(id)sender
{
    [NSApp unhide];
    [mainTabView selectTabViewItem:[mainTabView tabViewItemAtIndex:2]];
}

-(IBAction)visitHomePage:(id)sender
{
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"http://potatosaladx.blogspot.com/search/label/I8kfan"]];
}

@end
