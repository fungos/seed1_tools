//
//  Basic1AppDelegate.h
//  Basic1
//
//  Created by Danny Grein on 2/17/09.
//  Copyright TechFront Studios 2009. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <platform/iphone/IphoneView.h>
#include <Seed.h>

@interface SeedDelegate : NSObject <UIApplicationDelegate> 
{
    UIWindow *pWindow;
	SeedView *pView;
	SeedController *pController;
}

@end

