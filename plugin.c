/*
 * powermate plugin for VDR
 *
 * plugin.c - main plugin class
 *
 * Copyright (C) 2003 Andreas Regel <andreas.regel@powarman.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 */

#include <getopt.h>

#include "powermate.h"
#include "setup.h"

#include <vdr/plugin.h>

static const char *VERSION        = "0.0.1";
static const char *DESCRIPTION    = "Controls VDR through a Griffin PowerMate";
static const char *MAINMENUENTRY  = NULL;

class cPluginPowermate : public cPlugin
{
private:
	// Add any member variables or functions you may need here.
public:
	cPluginPowermate(void);
	virtual ~cPluginPowermate();
	virtual const char *Version(void) { return VERSION; }
	virtual const char *Description(void) { return DESCRIPTION; }
	virtual const char *CommandLineHelp(void);
	virtual bool ProcessArgs(int argc, char *argv[]);
	virtual bool Initialize(void);
	virtual bool Start(void);
	virtual void Housekeeping(void);
	virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
	virtual cOsdObject *MainMenuAction(void);
	virtual cMenuSetupPage *SetupMenu(void);
	virtual bool SetupParse(const char *Name, const char *Value);
};

cPluginPowermate::cPluginPowermate(void)
{
	// Initialize any member variables here.
	// DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
	// VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginPowermate::~cPluginPowermate()
{
	// Clean up after yourself!
	delete PowerMate;
}

const char *cPluginPowermate::CommandLineHelp(void)
{
	// Return a string that describes all known command line options.
	return "  -d DEV,   --device=DEV   use DEV as the PowerMate device (default is /dev/input/event0)\n";
}

bool cPluginPowermate::ProcessArgs(int argc, char *argv[])
{
	// Implement command line argument processing here if applicable.
	static struct option long_options[] = {
		{ "device", required_argument, NULL, 'd' },
		{ NULL }
	};
	int c;

	strcpy(PowerMateSetup.device, "/dev/input/event0");
	while ((c = getopt_long(argc, argv, "d:", long_options, NULL)) != -1)
	{
		switch (c)
		{
			case 'd':
				strcpy(PowerMateSetup.device, optarg);
				break;
			default:
				return false;
		}
	}
	return true;
}

bool cPluginPowermate::Initialize(void)
{
	// Initialize any background activities the plugin shall perform.
	return true;
}

bool cPluginPowermate::Start(void)
{
	// Start any background activities the plugin shall perform.
	PowerMate = new cPowerMate();
	return true;
}

void cPluginPowermate::Housekeeping(void)
{
	// Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginPowermate::MainMenuAction(void)
{
	// Perform the action when selected from the main VDR menu.
	return NULL;
}

cMenuSetupPage *cPluginPowermate::SetupMenu(void)
{
	// Return a setup menu in case the plugin supports one.
	return new cPowerMateSetupPage();
}

bool cPluginPowermate::SetupParse(const char *Name, const char *Value)
{
	// Parse your own setup parameters and store their values.
	if      (!strcasecmp(Name, "Brightness")) PowerMateSetup.brightness = atoi(Value);
	else if (!strcasecmp(Name, "Sensitivity")) PowerMateSetup.sensitivity = atoi(Value);
	else if (!strcasecmp(Name, "UsePushedTurns")) PowerMateSetup.usePushedTurns = atoi(Value);
	else if (!strcasecmp(Name, "KeyLeft")) PowerMateSetup.keyLeft = eKeys(atoi(Value));
	else if (!strcasecmp(Name, "KeyRight")) PowerMateSetup.keyRight = eKeys(atoi(Value));
	else if (!strcasecmp(Name, "KeyPush")) PowerMateSetup.keyPush = eKeys(atoi(Value));
	else if (!strcasecmp(Name, "KeyPushedLeft")) PowerMateSetup.keyPushedLeft = eKeys(atoi(Value));
	else if (!strcasecmp(Name, "KeyPushedRight")) PowerMateSetup.keyPushedRight = eKeys(atoi(Value));
	else
		return false;
	return true;
}

VDRPLUGINCREATOR(cPluginPowermate); // Don't touch this!
