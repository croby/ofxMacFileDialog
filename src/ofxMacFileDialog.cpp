/*
 *  ofxMacFileDialog.cpp
 *  ofxMacFileDialog
 *
 *  Created by Chris Roby on 12/28/09.
 *  Copyright 2009 Hill Holliday. All rights reserved.
 *
 *
 *	Some code from http://www.openframeworks.cc/forum/viewtopic.php?f=10&t=955
 *  and other pieces from Zach Lieberman http://snipt.org/lVl
 *  and even more from http://free.abracode.com/cyclone/sources/New%20Sources/NavDialogs.cp
 *  so......basically I just pieced this together
 */

#include "ofxMacFileDialog.h"

CFURLRef GetOpenDialogForUser(kDialogType type, char* title, char* message)
{
	NavDialogCreationOptions dialogOptions;
	NavDialogRef dialog = NULL;
	NavReplyRecord replyRecord;
	CFURLRef fileAsCFURLRef = NULL;
	FSRef fileAsFSRef;
	OSStatus status;
	
	// Get the standard set of defaults
	status = NavGetDefaultDialogCreationOptions(&dialogOptions);
//	require_noerr( status, CantGetNavOptions );
	
	dialogOptions.optionFlags = kNavNoTypePopup + kNavSupportPackages + kNavAllowOpenPackages;

	CFAllocatorRef alloc_default = kCFAllocatorDefault;  // = NULL;
	
	if (title != NULL) {
		CFStringRef cftitle = CFStringCreateWithCString(alloc_default,title,kCFStringEncodingMacRoman);
		dialogOptions.windowTitle = cftitle;
	}
	
	if (message != NULL) {
		CFStringRef cfmessage = CFStringCreateWithCString(alloc_default,message,kCFStringEncodingMacRoman);
		dialogOptions.message = cfmessage;
	}
	
	
	// Make the window app-wide modal
	dialogOptions.modality = kWindowModalityAppModal;
	
	// Create the dialog
	if (type == kDialogFile) {
		status = NavCreateGetFileDialog(&dialogOptions, NULL, NULL, NULL, NULL, NULL, &dialog);
	} else if (type == kDialogFolder) {
		status = NavCreateChooseFolderDialog(&dialogOptions, NULL, NULL, NULL, &dialog);
	}
	require_noerr( status, CantCreateDialog );
	
	// Show it
	status = NavDialogRun(dialog);
	require_noerr( status, CantRunDialog );
	
	// Get the reply
	status = NavDialogGetReply(dialog, &replyRecord);
	require( ((status == noErr) || (status == userCanceledErr)), CantGetReply );
	
	// If the user clicked "Cancel", just bail
	if ( status == userCanceledErr ) goto UserCanceled;
	
	// Get the file
	status = AEGetNthPtr(&(replyRecord.selection), 1, typeFSRef, NULL, NULL, &fileAsFSRef, sizeof(FSRef), NULL);
	require_noerr( status, CantExtractFSRef );
	
	// Convert it to a CFURL
	fileAsCFURLRef = CFURLCreateFromFSRef(NULL, &fileAsFSRef);
	
	// Cleanup
CantExtractFSRef:
UserCanceled:
	verify_noerr( NavDisposeReply(&replyRecord) );
CantGetReply:
CantRunDialog:
	NavDialogDispose(dialog);
CantCreateDialog:
CantGetNavOptions:
    return fileAsCFURLRef;
}


//--------------------------------------------------------------
string ofxMacFileDialog::getStringFromDialog(kDialogType type, char* windowTitle, char* windowMessage) {
	CFURLRef cfUrl = NULL;
	
	cfUrl = GetOpenDialogForUser(type, windowTitle, windowMessage);
	
	CFStringRef cfString = NULL;
	
	string fileNameToLoad = "";
	
	if ( cfUrl != NULL )
	{
		cfString = CFURLCopyFileSystemPath( cfUrl, kCFURLPOSIXPathStyle );
		CFRelease( cfUrl );
		
		// copy from a CFString into a local c string (http://www.carbondev.com/site/?page=CStrings+)
		const int kBufferSize = 255;
		
		char folderURL[kBufferSize];
		Boolean bool1 = CFStringGetCString(cfString,folderURL,kBufferSize,kCFStringEncodingMacRoman);

		
		string url1 = folderURL;
		string url2 = "";
		string finalURL = url1 + "/" + url2;
		fileNameToLoad = finalURL;
		
		if (fileNameToLoad.length() > 0)
			fileNameToLoad.erase(fileNameToLoad.length()-1);
	}
	
	return fileNameToLoad;
}

