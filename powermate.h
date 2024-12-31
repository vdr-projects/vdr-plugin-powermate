/*
 * powermate plugin for VDR
 *
 * powermate.h - PowerMate control class
 *
 * Copyright (C) 2003 Andreas Regel <andreas.regel@powarman.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 */

#ifndef _POWERMATE_H_
#define _POWERMATE_H_

#include <vdr/keys.h>
#include <vdr/thread.h>

class cPowerMate : private cThread
{
private:
	int fd;
	bool pushed;
	virtual void Action();
	void ProcessEvent(struct input_event * ev);
	void SendKey(eKeys key);
public:
	cPowerMate();
	virtual ~cPowerMate();
	void SetBrightness(int brightness);
};

extern cPowerMate * PowerMate;

#endif

