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

#ifndef __RCX_Target_h
#define __RCX_Target_h

#ifndef __RCX_Constants_h
#include "RCX_Constants.h"
#endif

typedef enum
{
	kRCX_RCXTarget = 0,	// original RCX
	kRCX_CMTarget,		// CyberMaster
	kRCX_ScoutTarget,	// Scout
	kRCX_RCX2Target,	// RCX 2.0
	kRCX_SpyboticsTarget,	// Spybotics
        kRCX_SwanTarget         // Dick Swan's alternate firmware
} RCX_TargetType;


class RCX_Target
{
public:
	struct Range {
		int	fBase;
		int	fCount;
	};

	RCX_TargetType	fType;
	const char*	fName;
	const char*	fDefine;
	const char*	fDefValue;

	Range		fRanges[kRCX_ChunkTypeCount];

	int		fMaxGlobalVars;
	int		fMaxTaskVars;
	bool		fResources;
	bool		fEvents;
	bool		fArrays;
        bool		fRestrictedMath;
        bool		fSubParams;
        bool		fHasExtendedMathOps;

	bool		SourceWritable(int source) const;
	int		GetChunkLimit(RCX_ChunkType type) const;
};


extern const RCX_Target *getTarget(RCX_TargetType type);


#endif
