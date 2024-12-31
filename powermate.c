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

#define NUM_VALID_PREFIXES 2
#define BUFFER_SIZE 32
#ifndef MSC_PULSELED
/* this may not have made its way into the kernel headers yet ... */
#define MSC_PULSELED 0x01
#endif

cPowerMate * PowerMate;

cPowerMate::cPowerMate()
:	pushed(false)
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
	Cancel();
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
	dsyslog("PowerMate thread started (pid=%d)", getpid());

	struct input_event events[BUFFER_SIZE];
	int r;
	int num;
	int i;

	for (; fd >= 0;)
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
					ProcessEvent(&events[i]);
				}
			}
			else
			{
				LOG_ERROR;
			}
		}
	}
	dsyslog("PowerMate thread ended (pid=%d)", getpid());
}

void cPowerMate::ProcessEvent(struct input_event * ev)
{
	static int offset = 0;

	switch (ev->type)
	{
		case EV_MSC:
			break;
		case EV_REL:
			if (ev->code != REL_DIAL)
			{
				fprintf(stderr, "powermate: Warning: unexpected rotation event; ev->code = 0x%04x\n", ev->code);
			}
			else
			{
				offset += ev->value;
				if (pushed)
				{
					if (PowerMateSetup.usePushedTurns)
					{
						if (ev->value < 0)
						{
							while (offset <= -PowerMateSetup.sensitivity)
							{
								SendKey(PowerMateSetup.keyPushedLeft);
								offset += PowerMateSetup.sensitivity;
							}
						}
						if (ev->value > 0)
						{
							while (offset >= PowerMateSetup.sensitivity)
							{
								SendKey(PowerMateSetup.keyPushedRight);
								offset -= PowerMateSetup.sensitivity;
							}
						}
					}
				}
				else
				{
					if (ev->value < 0)
					{
						while (offset <= -PowerMateSetup.sensitivity)
						{
							SendKey(PowerMateSetup.keyLeft);
							offset += PowerMateSetup.sensitivity;
						}
					}
					if (ev->value > 0)
					{
						while (offset >= PowerMateSetup.sensitivity)
						{
							SendKey(PowerMateSetup.keyRight);
							offset -= PowerMateSetup.sensitivity;
						}
					}
				}
			}
			break;
		case EV_KEY:
			if (ev->code != BTN_0)
			{
				fprintf(stderr, "powermate: Warning: unexpected key event; ev->code = 0x%04x\n", ev->code);
			}
			else
			{
				if (PowerMateSetup.usePushedTurns)
				{
					if (ev->value)
						pushed = true;
					else
						pushed = false;
				}
				else
				{
					if (ev->value)
						SendKey(PowerMateSetup.keyPush);
				}
			}
			break;
		default:
			fprintf(stderr, "powermate: Warning: unexpected event type; ev->type = 0x%04x\n", ev->type);
	}
}

void cPowerMate::SendKey(eKeys key)
{
	if (key != kNone)
		cRemote::Put(key);
}

