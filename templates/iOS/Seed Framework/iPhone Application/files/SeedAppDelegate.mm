//
//  Basic1AppDelegate.m
//  Basic1
//
//  Created by Danny Grein on 2/17/09.
//  Copyright TechFront Studios 2009. All rights reserved.
//

#import "SeedAppDelegate.h"
#import <Seed.h>

@implementation SeedDelegate

- (void)applicationDidFinishLaunching: (UIApplication *)application 
{
	//[application setStatusBarStyle:UIStatusBarStyleBlackTranslucent animated:NO];
	//[application setStatusBarHidden:YES withAnimation:UIStatusBarAnimationNone]; // Does not work with firmware bellow 3.2 and exceptions disabled!
	
	pWindow = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds]];
	pView = [[SeedView alloc] initWithFrame: [pWindow bounds]];
	
	pController = [[SeedController alloc] initWithView: pView];
	f32 time = 1.0f / (f32)pConfiguration->GetFrameRate();
	[pController SetUpdateRate: time];
	[pController Start];
	
	pWindow.backgroundColor = [UIColor yellowColor];
	[pWindow addSubview: pController.view];
	[pWindow makeKeyAndVisible];

	[[UIApplication sharedApplication] setIdleTimerDisabled: YES];
}


- (void)applicationWillResignActive: (UIApplication *)application 
{
	[pController Pause];
}


- (void)applicationDidBecomeActive: (UIApplication *)application 
{
}


- (void)applicationWillTerminate: (UIApplication *)application
{
	[pController Stop];
}


- (void)dealloc 
{
	[pWindow release];
	[pController release];
	[super dealloc];
}

@end
