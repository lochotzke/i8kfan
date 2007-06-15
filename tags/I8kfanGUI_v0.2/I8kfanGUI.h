//
//  I8kfanGUI.h
//  I8kfanGUI
//
//  Created by Andrew Bennett on 5/9/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <i8kfan.h>

@interface I8kfanGUI : NSObject {
    // bindings controller
    IBOutlet NSObjectController *fileOwnerController;

    // menus
    IBOutlet NSMenuItem *aboutMenuItem;
    NSStatusItem *statusItem;
    NSMenu *statusMenu;

    // views
    IBOutlet NSTabView *mainTabView;
    //IBOutlet NSTabViewItem *aboutTabViewItem;

    // text fields
    IBOutlet NSTextField *versionField;
    
    IBOutlet NSTextField *temperatureField;
    IBOutlet NSTextField *peakField;
    
    IBOutlet NSTextField *primaryFanSpeedField;
    IBOutlet NSTextField *primaryFanStatusField;
    IBOutlet NSTextField *secondaryFanSpeedField;
    IBOutlet NSTextField *secondaryFanStatusField;
    
    // input fields
    IBOutlet NSTextField *onThresholdField1;
    IBOutlet NSTextField *offThresholdField1;
    IBOutlet NSTextField *onThresholdField2;
    IBOutlet NSTextField *offThresholdField2;
    IBOutlet NSTextField *onThresholdField3;
    IBOutlet NSTextField *offThresholdField3;
    IBOutlet NSTextField *onThresholdField4;
    IBOutlet NSTextField *offThresholdField4;
    
    IBOutlet NSPopUpButton *thresholdList1;
    IBOutlet NSPopUpButton *thresholdList2;
    IBOutlet NSPopUpButton *thresholdList3;
    IBOutlet NSPopUpButton *thresholdList4;
    
    IBOutlet NSPopUpButton *thresholdList5;
    IBOutlet NSPopUpButton *thresholdList6;
    IBOutlet NSPopUpButton *thresholdList7;
    IBOutlet NSPopUpButton *thresholdList8;
    
    // check boxes
    IBOutlet NSButton *tempCheckBox;
    IBOutlet NSButton *overdriveCheckBox;
    
    // buttons
    IBOutlet NSSegmentedControl *primaryFanSegControl;
    IBOutlet NSSegmentedControl *secondaryFanSegControl;
    
    // records
    int peakCPUTemp;
    
    // update timer
    NSTimer *timer;
}

- (void)setPeakCPUTemp: (int)temp;
- (int)setPrimaryFanSpeedWithTemp: (int)temp status: (int)status;
- (int)getSpeedFromState: (int)state;

-(void)terminate:(id)sender;
-(void)setPrimaryFanOff:(id)sender;
-(void)setPrimaryFanLow:(id)sender;
-(void)setPrimaryFanHigh:(id)sender;
-(void)setSecondaryFanOff:(id)sender;
-(void)setSecondaryFanLow:(id)sender;
-(void)setSecondaryFanHigh:(id)sender;
-(IBAction)primaryFanSegControlClicked:(id)sender;
-(IBAction)secondaryFanSegControlClicked:(id)sender;
-(IBAction)showStatusPanel:(id)sender;
-(IBAction)showPreferencesPanel:(id)sender;
-(IBAction)showAboutPanel:(id)sender;
-(IBAction)visitHomePage:(id)sender;

@end
