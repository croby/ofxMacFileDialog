/*
 *  ofxMacFileDialog.h
 *  ofxMacFileDialog
 *
 *  Created by Chris Roby on 12/28/09.
 *  Copyright 2009 Hill Holliday. All rights reserved.
 *
 */

#include <string>
#include <CoreFoundation/CoreFoundation.h>
#include <Carbon/Carbon.h>
using namespace std;

enum kDialogType {
	kDialogFile,
	kDialogFolder
};

class ofxMacFileDialog {
public:
	string getStringFromDialog(kDialogType type, char* windowTitle, char* windowMessage);
};
