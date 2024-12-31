/*
 * powermate plugin for VDR
 *
 * setup.c - settings and setup menu
 *
 * Copyright (C) 2003 Andreas Regel <andreas.regel@powarman.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 */

#include "powermate.h"
#include "setup.h"

cPowerMateSetup::cPowerMateSetup()
:	brightness(0),
	sensitivity(1),
	usePushedTurns(false),
	keyLeft(kNone),
	keyRight(kNone),
	keyPush(kNone),
	keyPushedLeft(kNone),
	keyPushedRight(kNone)
{
}

static tKey keyTable[] = {
                    { kNone,          "None"       },
                    { kUp,            "Up"         },
                    { kDown,          "Down"       },
                    { kMenu,          "Menu"       },
                    { kOk,            "Ok"         },
                    { kBack,          "Back"       },
                    { kLeft,          "Left"       },
                    { kRight,         "Right"      },
                    { kRed,           "Red"        },
                    { kGreen,         "Green"      },
                    { kYellow,        "Yellow"     },
                    { kBlue,          "Blue"       },
                    { k0,             "0"          },
                    { k1,             "1"          },
                    { k2,             "2"          },
                    { k3,             "3"          },
                    { k4,             "4"          },
                    { k5,             "5"          },
                    { k6,             "6"          },
                    { k7,             "7"          },
                    { k8,             "8"          },
                    { k9,             "9"          },
                    { kPlay,          "Play"       },
                    { kPause,         "Pause"      },
                    { kStop,          "Stop"       },
                    { kRecord,        "Record"     },
                    { kFastFwd,       "FastFwd"    },
                    { kFastRew,       "FastRew"    },
                    { kPower,         "Power"      },
                    { kChanUp,        "Channel+"   },
                    { kChanDn,        "Channel-"   },
                    { kVolUp,         "Volume+"    },
                    { kVolDn,         "Volume-"    },
                    { kMute,          "Mute"       },
                    { kSchedule,      "Schedule"   },
                    { kChannels,      "Channels"   },
                    { kTimers,        "Timers"     },
                    { kRecordings,    "Recordings" },
                    { kSetup,         "Setup"      },
                    { kCommands,      "Commands"   },
                    { kUser1,         "User1"      },
                    { kUser2,         "User2"      },
                    { kUser3,         "User3"      },
                    { kUser4,         "User4"      },
                    { kUser5,         "User5"      },
                    { kUser6,         "User6"      },
                    { kUser7,         "User7"      },
                    { kUser8,         "User8"      },
                    { kUser9,         "User9"      },
                    { kNone,          NULL         }
};

cPowerMateSetupPage::cPowerMateSetupPage()
{
	brightness = PowerMateSetup.brightness;
	sensitivity = PowerMateSetup.sensitivity;
	usePushedTurns = PowerMateSetup.usePushedTurns;
	for (numKeys = 0; keyTable[numKeys].name; numKeys++)
	{
		if (keyTable[numKeys].type == PowerMateSetup.keyLeft)
			keyLeft = numKeys;
		if (keyTable[numKeys].type == PowerMateSetup.keyRight)
			keyRight = numKeys;
		if (keyTable[numKeys].type == PowerMateSetup.keyPush)
			keyPush = numKeys;
		if (keyTable[numKeys].type == PowerMateSetup.keyPushedLeft)
			keyPushedLeft = numKeys;
		if (keyTable[numKeys].type == PowerMateSetup.keyPushedRight)
			keyPushedRight = numKeys;
		strKeys[numKeys] = new char[strlen(keyTable[numKeys].name) + 1];
		strcpy(strKeys[numKeys], keyTable[numKeys].name);
	}
	Add(itemBrightness = new cMenuEditIntItem("Brightness", &brightness, 0, 255));
	Add(new cMenuEditIntItem("Sensitivity", &sensitivity, 1, 10));
	Add(new cMenuEditBoolItem("Use Pushed Turns", &usePushedTurns));
	Add(new cMenuEditStraItem("Turn Left", &keyLeft, numKeys, strKeys));
	Add(new cMenuEditStraItem("Turn Right", &keyRight, numKeys, strKeys));
	Add(new cMenuEditStraItem("Push Button", &keyPush, numKeys, strKeys));
	Add(new cMenuEditStraItem("Pushed Turn Left", &keyPushedLeft, numKeys, strKeys));
	Add(new cMenuEditStraItem("Pushed Turn Right", &keyPushedRight, numKeys, strKeys));
}

cPowerMateSetupPage::~cPowerMateSetupPage()
{
	for (int i = 0; i < numKeys; i++)
	{
		delete[] strKeys[i];
	}
}

void cPowerMateSetupPage::Store()
{
	SetupStore("Brightness", PowerMateSetup.brightness = brightness);
	SetupStore("Sensitivity", PowerMateSetup.sensitivity = sensitivity);
	SetupStore("UsePushedTurns", PowerMateSetup.usePushedTurns = usePushedTurns);
	SetupStore("KeyLeft", PowerMateSetup.keyLeft = keyTable[keyLeft].type);
	SetupStore("KeyRight", PowerMateSetup.keyRight = keyTable[keyRight].type);
	SetupStore("KeyPush", PowerMateSetup.keyPush = keyTable[keyPush].type);
	SetupStore("KeyPushedLeft", PowerMateSetup.keyPushedLeft = keyTable[keyPushedLeft].type);
	SetupStore("KeyPushedRight", PowerMateSetup.keyPushedRight = keyTable[keyPushedRight].type);

	PowerMate->SetBrightness(brightness);
}

eOSState cPowerMateSetupPage::ProcessKey(eKeys key)
{
	eOSState state = cMenuSetupPage::ProcessKey(key);

	if (state == osContinue)
	{
		cOsdItem * item = Get(Current());
		if (item == itemBrightness)
		{
			PowerMate->SetBrightness(brightness);
		}
	}
	return state;
}

cPowerMateSetup PowerMateSetup;

