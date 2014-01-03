/*
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Initial Developer of this code is David Baum.
 * Portions created by David Baum are Copyright (C) 1998 David Baum.
 * All Rights Reserved.
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */

#include "RCX_Target.h"
#include "RCX_Constants.h"

static const class RCX_Target sTargets[] = {
	{
		kRCX_RCXTarget, "RCX", "__RCX", "1",
		{ 0, 10, 0, 8, 0, 0, 0, 0 },
		32, 0, false, false, false, true, false, false
	},

	{
		kRCX_CMTarget, "CyberMaster", "__CM", "1",
		{ 0, 4,	0, 4, 0, 0, 0, 0 },
		32, 0, false, false, false, true, false, false
	},

	{
		kRCX_ScoutTarget, "Scout", "__SCOUT", "1",
		{ 0, 6,	0, 3, 0, 0, 0, 0 },
		10, 8, true, true, false, true, false, false
	},

	{
		kRCX_RCX2Target, "RCX2", "__RCX", "2",
		{ 0, 10, 0, 8, 0, 0, 0, 0 },
		32, 16, true, true, true, false, false, false
	},

        {
                kRCX_SpyboticsTarget, "Spybotics", "__SPY", "1",
                { 8, 8, 128, 32, 64, 16, 8, 8 },
                32, 4, true, true, true, false, true, false
        },

        {
                kRCX_SwanTarget, "Swan", "__SWAN", "1",
                { 0, 10, 0, 8, 0, 0, 0, 0 },
                256, 0, true, true, true, false, true, true
        },
};

const RCX_Target *getTarget(RCX_TargetType type)
{
	// don't assume order of array matches types
	for(unsigned i=0; i<sizeof(sTargets)/sizeof(RCX_Target); ++i)
		if (sTargets[i].fType == type) return &sTargets[i];

	return 0;
};


bool RCX_Target::SourceWritable(int source) const
{
	// vars are always writable
	if (source == kRCX_VariableType) return true;

	// only RCX2/Spybotics support generic writable sources
	if (fType != kRCX_RCX2Target &&
            fType != kRCX_SpyboticsTarget &&
            fType != kRCX_SwanTarget) return false;

	switch(source)
	{
		// these sources are writable for RCX 2
		case 0:	// shouldn't need this one..vars handled above
		case 1: // timer
		case 4: // random seed
		case 8: // program slot
		case 10: // sensor type
		case 11: // sensor mode
		case 14: // watch
		case 21: // counter
		case 26: // fast timer
		case 28: // upper threshold
		case 29: // lower threshold
		case 30: // hysteresis
		case 31: // duration
		case 33: // UART settings
		case 36: // indirect
			return true;

                case 15: // Message
                case 27: // Click counter
                case 37: // datalog type indirect (RCX2 and Swan)
                case 38: // datalog type direct (RCX2 and Swan)
                case 39: // datalog value indirect (RCX2 and Swan)
                case 40: // datalog value direct (RCX2 and Swan)
                case 41: // datalog byte indirect (RCX2 and Swan)
                case 42: // datalog byte direct (RCX2 and Swan)
                        return (fType == kRCX_RCX2Target) || (fType == kRCX_SwanTarget);

                case 18: // Spybot Stack or Event Type (Swan)
                case 19: // Spybot Timer control or Event (Swan)
                case 20: // Spybot EEPROM or Event Counts (Swan)
                case 22: // Spybot LED or 1 MSec Timer (Swan)
                case 43: // Spybot game notes or Global Var (Swan)
                case 50: // Spybot target info or Constant Var (Swan)
                case 51: // Spybot Ping Control or Function Return Value Word (Swan)
                case 52: // Spybot Beacon Control or Function Return Value Long (Swan)
                case 53: // Spybot Sound Control or Function Return Value Float (Swan)
                case 54: // Spybot Indirect EEPROM or Var Byte (Swan)
                        return (fType == kRCX_SpyboticsTarget) || (fType == kRCX_SwanTarget);

                case 3: // Motor State (Swan makes this source writeable)
                case 5: // Motor Power Signed (Swan)
                case 6: // Intrinsic Indirect Global (Swan)
                case 7: // Motor Brake Power (Swan)
                case 16: // Motor Power 128 (Swan)
                case 17: // Global Motor State (Swan makes this source writeable)
                case 24: // System (Swan)
                case 32: // Motor Power 8 (Swan)
                case 44: // Indirect Global Int (Swan)
                case 45: // Indirect Global Long (Swan)
                case 46: // Indirect Global Float (Swan)
                case 47: // Indexed Global and Constant (Swan)
                case 48: // Indexed Global Long and Constant (Swan)
                case 49: // Stack Var (Swan)
                case 55: // Var Word (Swan)
                case 56: // Var Long (Swan)
                case 57: // Task Stack Var Byte (Swan)
                case 58: // Task Stack Var Word (Swan)
                case 59: // Task Stack Var Long (Swan)
                case 60: // Task Var (Swan)
                case 61: // Task Stack Address (Swan)
                case 62: // Task Stack Size (Swan)
                        return fType == kRCX_SwanTarget;

	}

	return false;
}


int RCX_Target::GetChunkLimit(RCX_ChunkType type) const
{
	if (type<0 || type>= kRCX_ChunkTypeCount) return 0;

	return fRanges[type].fBase + fRanges[type].fCount;
}
