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


const int kEvents = 6;
const int kEventTurnLeft        = 0;
const int kEventTurnRight       = 1;
const int kEventPushedTurnLeft  = 2;
const int kEventPushedTurnRight = 3;
const int kEventClick           = 4;
const int kEventDoubleClick     = 5;

const int kContexts = 3;
const int kContextNormal = 0;
const int kContextMenu   = 1;
const int kContextReplay = 2;

class cPowerMateSetup
{
public:
    char device[256];
    int brightness;
    int sensitivity;
    int doubleClickTime;
    eKeys keys[kContexts][kEvents];
public:
    cPowerMateSetup();
    bool Parse(const char *Name, const char *Value);
};

class cPowerMateSetupPage : public cMenuSetupPage
{
private:
    const char * strKeys[100];
    const char * strContexts[kContexts];
    int numKeys;
    int brightness;
    int sensitivity;
    int doubleClickTime;
    int keys[kContexts][kEvents];
    int setKeys[kEvents];
    int context;
    int contextOld;

    cOsdItem * itemBrightness;
    cOsdItem * itemContext;
protected:
    virtual void Store();
    virtual eOSState ProcessKey(eKeys key);
public:
    cPowerMateSetupPage();
    virtual ~cPowerMateSetupPage();
};


extern cPowerMateSetup PowerMateSetup;

#endif

