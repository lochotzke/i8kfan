//
//  main.m
//  I8kfanGUI
//
//  Created by Andrew Bennett on 5/9/07.
//  Copyright __MyCompanyName__ 2007. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <security/security.h>

int main(int argc, char *argv[])
{
    const char *pathToSelf; 
	int res;

	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	
	FILE *f;

	pathToSelf = [[[NSBundle mainBundle] executablePath] UTF8String];
	
	AuthorizationRef auth;
	res = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &auth);

	setuid(0);
	if (getuid() == 0)
	{
		res = NSApplicationMain(argc,  (const char **) argv);
	}
	else
	{
		AuthorizationExecuteWithPrivileges(auth, pathToSelf, kAuthorizationFlagDefaults, argv, &f);
		//printf("%08x\n", f);
		res = 0;
	}
	
	[pool release];

	return res;
}
