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
#include <vdr/status.h>
#include <vdr/thread.h>
#include <vdr/tools.h>

class cPowerMate : public cThread, cStatus
{
private:
    int fd;

    int context;
    bool menu;

    bool pushed;
    uint64_t lastPush;
    int offset;
    bool click;
    bool turn;

    bool active;

    virtual void Action();
    void ProcessInputEvent(struct input_event * ev);
    void ProcessEvent(int event);
    void SendKey(eKeys key);
public:
    cPowerMate();
    virtual ~cPowerMate();
    void SetBrightness(int brightness);

    virtual void Replaying(const cControl *Control, const char *Name, const char *FileName, bool On);
    virtual void OsdClear(void);
    virtual void OsdTitle(const char *Title);
    virtual void OsdHelpKeys(const char *Red, const char *Green, const char *Yellow, const char *Blue);
    virtual void OsdItem(const char *Text, int Index);
    virtual void OsdCurrentItem(const char *Text);
    virtual void OsdTextItem(const char *Text, bool Scroll);
};

extern cPowerMate * PowerMate;

#endif

