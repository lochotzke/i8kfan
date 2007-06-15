#import "KeyChain.h"
#import <Security/Security.h> 


@implementation KeyChain

+(NSDictionary *)accessToKeyChain:(NSString *)mode user:(NSString*)user pw:(NSString*)pw {
	
	SecKeychainAttribute attributes[3];
    SecKeychainAttributeList list,xlist;
    OSStatus status;
	NSMutableDictionary *returndata;
	attributes[0].tag = kSecAccountItemAttr;
	NSString *login;
	SecKeychainItemRef item;

	
	login=user;
	const char* clogin;
	int clength=[login length];
	clogin=[login UTF8String];
	attributes[0].data =(void*)clogin;
	attributes[0].length = clength;
    
    attributes[1].tag = kSecDescriptionItemAttr;
    attributes[1].data = "Application-Password";
    attributes[1].length = strlen(attributes[1].data);

    attributes[2].tag = kSecLabelItemAttr;
    attributes[2].data = "i8kfangui";
    attributes[2].length = strlen(attributes[2].data);
	
	list.count = 3;
    list.attr = attributes;
	
	returndata=[[NSMutableDictionary alloc] init];

	if ([mode isEqualToString:@"Save"]){
	
		NSString *password=pw;
		const char* cpassw;
		cpassw=[password UTF8String];
		status = SecKeychainItemCreateFromContent(kSecGenericPasswordItemClass, &list,
													strlen(cpassw), cpassw, NULL,NULL,&item);
		if (status == errSecDuplicateItem) { //item exits, update it
			SecKeychainSearchRef search;
			SecKeychainItemRef olditem;
			status = SecKeychainSearchCreateFromAttributes (NULL, kSecGenericPasswordItemClass,&list, &search);
			status = SecKeychainSearchCopyNext (search, &olditem);
			//NSLog(@"Error during keychain access %d",status);
			if (status==noErr) {
				status=SecKeychainItemModifyContent (olditem,&list,strlen(cpassw),cpassw);
				CFRelease(olditem);
			}
		}
	} else {
	  //load from keychain and set attributes
		void* outData = nil;
		UInt32 len;
		SecKeychainSearchRef search;
		SecKeychainItemRef founditem;
		SecKeychainAttribute outList[] =
		{
		{kSecAccountItemAttr,},
		{kSecDescriptionItemAttr,},
		{kSecLabelItemAttr}
		};
		xlist.count = sizeof(outList)/sizeof(outList[0]);
		xlist.attr = outList;
		status = SecKeychainSearchCreateFromAttributes (NULL, kSecGenericPasswordItemClass,&list, &search);
		//NSLog(@"Tell me: %d",status);
		status=SecKeychainSearchCopyNext (search, &founditem);
		if (status!=errSecItemNotFound) {
			status = SecKeychainItemCopyContent (founditem, NULL, &xlist, &len, &outData);
			NSString *password;
			password = [[NSString alloc]  initWithBytes:outData length:len encoding:NSUTF8StringEncoding];
			[returndata setObject:password forKey:@"Password"];
			[password release];
			CFRelease(founditem);
		}	
	}
	return returndata;
}

@end
