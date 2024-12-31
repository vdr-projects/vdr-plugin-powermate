/*
 * powermate plugin for VDR
 *
 * powermate.c - PowerMate control class
 *
 * Copyright (C) 2003 Andreas Regel <andreas.regel@powarman.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 */

#include <linux/input.h>
#include <string.h>
#include <errno.h>

#include "powermate.h"
#include "setup.h"

#include <vdr/remote.h>
#include <vdr/tools.h>

#define NUM_VALID_PREFIXES 2
#define BUFFER_SIZE 32
#ifndef MSC_PULSELED
/* this may not have made its way into the kernel headers yet ... */
#define MSC_PULSELED 0x01
#endif

cPowerMate * PowerMate;

cPowerMate::cPowerMate()
:   context(kContextNormal),
    menu(false),
    pushed(false),
    lastPush(0),
    offset(0),
    click(false),
    turn(false),
    active(false)
{
    static const char * valid_prefix[NUM_VALID_PREFIXES] =
    {
        "Griffin PowerMate",
        "Griffin SoundKnob"
    };

    char name[255];
    int i;

    fd = open(PowerMateSetup.device, O_RDWR);
    if (fd < 0)
    {
        esyslog("powermate: unable to open %s: %s\n", PowerMateSetup.device, strerror(errno));
        fd = -1;
        return;
    }

    if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0)
    {
        esyslog("powermate: %s: EVIOCGNAME failed: %s\n", PowerMateSetup.device, strerror(errno));
        close(fd);
        fd = -1;
        return;
    }

    // it's the correct device if the prefix matches what we expect it to be:
    for (i = 0; i < NUM_VALID_PREFIXES; i++)
        if (!strncasecmp(name, valid_prefix[i], strlen(valid_prefix[i])))
            break;

    if (i == NUM_VALID_PREFIXES)
    {
        esyslog("powermate: unable to locate PowerMate at %s\n", PowerMateSetup.device);
        close(fd);
        fd = -1;
        return;
    }

    SetBrightness(PowerMateSetup.brightness);
    Start();
}

cPowerMate::~cPowerMate()
{
    active = false;
    Cancel(3);
}

void cPowerMate::SetBrightness(int brightness)
{
    struct input_event ev;

    ev.type = EV_MSC;
    ev.code = MSC_PULSELED;
    ev.value = brightness;

    write(fd, &ev, sizeof(struct input_event));
}

void cPowerMate::Action()
{
    active = true;
    dsyslog("PowerMate thread started (pid=%d)", getpid());

    struct input_event events[BUFFER_SIZE];
    int r;
    int num;
    int i;

    while (fd >= 0 && active)
    {
        LOCK_THREAD;
        if (cFile::FileReady(fd, 100))
        {
            r = safe_read(fd, events, sizeof(struct input_event) * BUFFER_SIZE);
            if (r > 0)
            {
                num = r / sizeof(struct input_event);
                for (i = 0; i < num; i++)
                {
                    ProcessInputEvent(&events[i]);
                }
            }
            else
            {
                LOG_ERROR;
            }
        }
        if (click && !pushed)
        {
            uint64_t current = cTimeMs::Now();
            if (current - lastPush > (uint64_t) PowerMateSetup.doubleClickTime)
            {
                //printf("CLICK\n");
                ProcessEvent(kEventClick);
                click = false;
            }
        }
    }
    dsyslog("PowerMate thread ended (pid=%d)", getpid());
}

void cPowerMate::ProcessInputEvent(struct input_event * ev)
{
    switch (ev->type)
    {
        case EV_MSC:
            break;
        case EV_REL:
            if (ev->code != REL_DIAL)
            {
                fprintf(stderr, "powermate: Warning: unexpected rotation event; ev->code = 0x%04x\n", ev->code);
                break;
            }
            turn = true;
            offset += (int) ev->value;
            if (pushed)
            {
                if ((int) ev->value < 0)
                {
                    while (offset <= -PowerMateSetup.sensitivity)
                    {
                        ProcessEvent(kEventPushedTurnLeft);
                        offset += PowerMateSetup.sensitivity;
                    }
                }
                if ((int) ev->value > 0)
                {
                    while (offset >= PowerMateSetup.sensitivity)
                    {
                        ProcessEvent(kEventPushedTurnRight);
                        offset -= PowerMateSetup.sensitivity;
                    }
                }
            }
            else
            {
                if (click)
                {
                    //printf("CLICK\n");
                    ProcessEvent(kEventClick);
                    click = false;
                }
                if ((int) ev->value < 0)
                {
                    while (offset <= -PowerMateSetup.sensitivity)
                    {
                        ProcessEvent(kEventTurnLeft);
                        offset += PowerMateSetup.sensitivity;
                    }
                }
                if ((int) ev->value > 0)
                {
                    while (offset >= PowerMateSetup.sensitivity)
                    {
                        ProcessEvent(kEventTurnRight);
                        offset -= PowerMateSetup.sensitivity;
                    }
                }
            }
            break;
        case EV_KEY:
            if (ev->code != BTN_0)
            {
                fprintf(stderr, "powermate: Warning: unexpected key event; ev->code = 0x%04x\n", ev->code);
                break;
            }
            if (ev->value)
            {
                pushed = true;
                turn = false;
                offset = 0;
                lastPush = cTimeMs::Now();
            }
            else
            {
                if (click)
                {
                    //printf("DOUBLECLICK\n");
                    ProcessEvent(kEventDoubleClick);
                    click = false;
                }
                else
                {
                    if (!turn)
                        click = true;
                }
                pushed = false;
                turn = false;
                offset = 0;
            }
            break;
    }
}

void cPowerMate::ProcessEvent(int event)
{
    if (menu)
        SendKey(PowerMateSetup.keys[kContextMenu][event]);
    else
        SendKey(PowerMateSetup.keys[context][event]);
}

void cPowerMate::SendKey(eKeys key)
{
    if (key != kNone)
        cRemote::Put(key);
}

void cPowerMate::Replaying(const cControl *Control, const char *Name, const char *FileName, bool On)
{
    if (On)
    {
        context = kContextReplay;
    }
    else
    {
        context = kContextNormal;
    }
}

void cPowerMate::OsdClear(void)
{
    menu = false;
}

void cPowerMate::OsdTitle(const char *Title)
{
    menu = true;
}

void cPowerMate::OsdHelpKeys(const char *Red, const char *Green, const char *Yellow, const char *Blue)
{
    menu = true;
}

void cPowerMate::OsdItem(const char *Text, int Index)
{
    menu = true;
}

void cPowerMate::OsdCurrentItem(const char *Text)
{
    menu = true;
}

void cPowerMate::OsdTextItem(const char *Text, bool Scroll)
{
    menu = true;
}

