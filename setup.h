/*
 * powermate plugin for VDR
 *
 * setup.h - settings and setup menu
 *
 * Copyright (C) 2003 Andreas Regel <andreas.regel@powarman.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 */

#ifndef _SETUP_H_
#define _SETUP_H_

#include <vdr/plugin.h>

class cPowerMateSetup
{
public:
	char device[256];
	int brightness;
	int sensitivity;
	bool usePushedTurns;
	eKeys keyLeft;
	eKeys keyRight;
	eKeys keyPush;
	eKeys keyPushedLeft;
	eKeys keyPushedRight;
public:
	cPowerMateSetup();
};

class cPowerMateSetupPage : public cMenuSetupPage
{
private:
	char * strKeys[100];
	int numKeys;
	int brightness;
	int sensitivity;
	int usePushedTurns;
	int keyLeft;
	int keyRight;
	int keyPush;
	int keyPushedLeft;
	int keyPushedRight;

	cOsdItem * itemBrightness;
protected:
	virtual void Store();
	virtual eOSState ProcessKey(eKeys key);
public:
	cPowerMateSetupPage();
	virtual ~cPowerMateSetupPage();
};


extern cPowerMateSetup PowerMateSetup;

#endif

