#pragma once
#include "LynxStructure.h"

using namespace LynxStructureSpace;


//------------------- Example of a basic struct -----------------------------

#define CONTROLLER_STRUCT 0x12
enum ControllerContents
{
	joy_LX,
	joy_LY,
	joy_RX,
	joy_RY,
	trigger_L,
	trigger_R,
	buttons,
	endOfList
};

static const StructItem controllerItems[]
{
	{ "joy_LX", eInt16 },
	{ "joy_LY", eInt16 },
	{ "joy_RX", eInt16 },
	{ "joy_RY", eInt16 },
	{ "trigger_L", eUint8 },
	{ "trigger_R", eUint8 },
	{ "buttons", eUint32},
	{ "", eEndOfList }
};

static const StructDefinition controllerDefinition
{
	 "Controller Struct",
	 eStructureMode,
	 controllerItems
};


//---------------- Example of an array struct ----------------------------

#define CONTROLLER_ARRAY 0x13
static const StructItem controllerArrayItems[]
{
	{ "", eInt32 }
};

static const StructDefinition controllerArray
{
	{ "Controller Array" },
	{ eArrayMode },
	{ controllerArrayItems },
	{ 10 }
};