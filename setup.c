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

const char * kStrContexts[kContexts] =
{
    trNOOP("Normal"),
    trNOOP("Menu"),
    trNOOP("Replay")
};

const char * kStrEvents[kEvents] =
{
    "TurnLeft",
    "TurnRight",
    "PushedTurnLeft",
    "PushedTurnRight",
    "Click",
    "DoubleClick"
};

cPowerMateSetup::cPowerMateSetup()
:   brightness(0),
    sensitivity(1),
    doubleClickTime(300)
{
    for (int c = 0; c < kContexts; c++)
        for (int e = 0; e < kEvents; e++)
            keys[c][e] = kNone;
}

bool cPowerMateSetup::Parse(const char *Name, const char *Value)
{
    // Parse your own setup parameters and store their values.
    if      (!strcasecmp(Name, "Brightness")) brightness = atoi(Value);
    else if (!strcasecmp(Name, "Sensitivity")) sensitivity = atoi(Value);
    else if (!strcasecmp(Name, "DoubleClickTime")) doubleClickTime = atoi(Value);
    else if (strstr(Name, "Key") == Name)
    {
        for (int c = 0; c < kContexts; c++)
        {
            for (int e = 0; e < kEvents; e++)
            {
                char str[256];
                sprintf(str, "Key%s%s", kStrContexts[c], kStrEvents[e]);
                if (!strcasecmp(Name, str))
                {
                    keys[c][e] = eKeys(atoi(Value));
                    return true;
                }
            }
        }
        return false;
    }
    else
        return false;
    return true;
}

static tKey keyTable[] = {
    { kNone,          "none"           },
    { kUp,            "Key$Up"         },
    { kDown,          "Key$Down"       },
    { kMenu,          "Key$Menu"       },
    { kOk,            "Key$Ok"         },
    { kBack,          "Key$Back"       },
    { kLeft,          "Key$Left"       },
    { kRight,         "Key$Right"      },
    { kRed,           "Key$Red"        },
    { kGreen,         "Key$Green"      },
    { kYellow,        "Key$Yellow"     },
    { kBlue,          "Key$Blue"       },
    { k0,             "Key$0"          },
    { k1,             "Key$1"          },
    { k2,             "Key$2"          },
    { k3,             "Key$3"          },
    { k4,             "Key$4"          },
    { k5,             "Key$5"          },
    { k6,             "Key$6"          },
    { k7,             "Key$7"          },
    { k8,             "Key$8"          },
    { k9,             "Key$9"          },
    { kInfo,          "Key$Info"       },
    { kPlay,          "Key$Play"       },
    { kPause,         "Key$Pause"      },
    { kStop,          "Key$Stop"       },
    { kRecord,        "Key$Record"     },
    { kFastFwd,       "Key$FastFwd"    },
    { kFastRew,       "Key$FastRew"    },
    { kNext,          "Key$Next"       },
    { kPrev,          "Key$Prev"       },
    { kPower,         "Key$Power"      },
    { kChanUp,        "Key$Channel+"   },
    { kChanDn,        "Key$Channel-"   },
    { kChanPrev,      "Key$PrevChannel"},
    { kVolUp,         "Key$Volume+"    },
    { kVolDn,         "Key$Volume-"    },
    { kMute,          "Key$Mute"       },
    { kAudio,         "Key$Audio"      },
#if (VDRVERSNUM >= 10510)
    { kSubtitles,     "Key$Subtitles"  },
#endif
    { kSchedule,      "Key$Schedule"   },
    { kChannels,      "Key$Channels"   },
    { kTimers,        "Key$Timers"     },
    { kRecordings,    "Key$Recordings" },
    { kSetup,         "Key$Setup"      },
    { kCommands,      "Key$Commands"   },
    { kUser1,         "Key$User1"      },
    { kUser2,         "Key$User2"      },
    { kUser3,         "Key$User3"      },
    { kUser4,         "Key$User4"      },
    { kUser5,         "Key$User5"      },
    { kUser6,         "Key$User6"      },
    { kUser7,         "Key$User7"      },
    { kUser8,         "Key$User8"      },
    { kUser9,         "Key$User9"      },
    { kNone,          NULL             }
};

cPowerMateSetupPage::cPowerMateSetupPage()
{
    context = kContextNormal;
    contextOld = kContextNormal;
    brightness = PowerMateSetup.brightness;
    sensitivity = PowerMateSetup.sensitivity;
    doubleClickTime = PowerMateSetup.doubleClickTime;
    for (numKeys = 0; keyTable[numKeys].name; numKeys++)
    {
        for (int c = 0; c < kContexts; c++)
            for (int e = 0; e < kEvents; e++)
                if (keyTable[numKeys].type == PowerMateSetup.keys[c][e])
                    keys[c][e] = numKeys;
        strKeys[numKeys] = trVDR(keyTable[numKeys].name);
    }
    for (int c = 0; c < kContexts; c++)
    {
        strContexts[c] = tr(kStrContexts[c]);
    }
    for (int e = 0; e < kEvents; e++)
        setKeys[e] = keys[context][e];
    Add(itemBrightness = new cMenuEditIntItem(tr("Brightness"), &brightness, 0, 255));
    Add(new cMenuEditIntItem(tr("Sensitivity"), &sensitivity, 1, 10));
    Add(new cMenuEditIntItem(tr("Double click time (ms)"), &doubleClickTime, 100, 1000));
    Add(itemContext = new cMenuEditStraItem(tr("Set keys for context"), &context, kContexts, strContexts));
    Add(new cMenuEditStraItem(tr("Turn Left"), &setKeys[kEventTurnLeft], numKeys, strKeys));
    Add(new cMenuEditStraItem(tr("Turn Right"), &setKeys[kEventTurnRight], numKeys, strKeys));
    Add(new cMenuEditStraItem(tr("Pushed Turn Left"), &setKeys[kEventPushedTurnLeft], numKeys, strKeys));
    Add(new cMenuEditStraItem(tr("Pushed Turn Right"), &setKeys[kEventPushedTurnRight], numKeys, strKeys));
    Add(new cMenuEditStraItem(tr("Button Click"), &setKeys[kEventClick], numKeys, strKeys));
    Add(new cMenuEditStraItem(tr("Button Double Click"), &setKeys[kEventDoubleClick], numKeys, strKeys));
}

cPowerMateSetupPage::~cPowerMateSetupPage()
{
}

void cPowerMateSetupPage::Store()
{
    SetupStore("Brightness", PowerMateSetup.brightness = brightness);
    SetupStore("Sensitivity", PowerMateSetup.sensitivity = sensitivity);
    SetupStore("DoubleClickTime", PowerMateSetup.doubleClickTime = doubleClickTime);
    
    for (int e = 0; e < kEvents; e++)
        keys[contextOld][e] = setKeys[e];
    for (int c = 0; c < kContexts; c++)
    {
        for (int e = 0; e < kEvents; e++)
        {
            char str[256];
            PowerMateSetup.keys[c][e] = keyTable[keys[c][e]].type;
            sprintf(str, "Key%s%s", kStrContexts[c], kStrEvents[e]);
            SetupStore(str, PowerMateSetup.keys[c][e]);
        }
    }

    PowerMate->SetBrightness(brightness);
}

eOSState cPowerMateSetupPage::ProcessKey(eKeys key)
{
    eOSState state = cMenuSetupPage::ProcessKey(key);

    if (state == osContinue)
    {
        if (key == kLeft || key == kRight)
        {
            cOsdItem * item = Get(Current());
            if (item == itemBrightness)
            {
                PowerMate->SetBrightness(brightness);
            }
            if (item == itemContext)
            {
                for (int e = 0; e < kEvents; e++)
                {
                    keys[contextOld][e] = setKeys[e];
                    setKeys[e] = keys[context][e];
                    item = Get(Current() + e + 1);
                    item->Set();
                }
                contextOld = context;
                Display();
            }
        }
    }
    return state;
}

cPowerMateSetup PowerMateSetup;

