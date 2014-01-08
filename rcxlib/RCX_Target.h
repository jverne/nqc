/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The Initial Developer of this code is David Baum.
 * Portions created by David Baum are Copyright (C) 1999 David Baum.
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

typedef enum {
    kRCX_RCXTarget = 0,     // Original RCX
    kRCX_CMTarget,          // CyberMaster
    kRCX_ScoutTarget,       // Scout
    kRCX_RCX2Target,        // RCX 2.0
    kRCX_SpyboticsTarget,   // Spybotics
    kRCX_SwanTarget         // Dick Swan's alternate firmware
} RCX_TargetType;


class RCX_Target
{
public:
    struct Range {
        int fBase;
        int fCount;
    };

    RCX_TargetType fType;
    const char* fName;
    const char* fDefine;
    const char* fDefValue;

    Range fRanges[kRCX_ChunkTypeCount];

    int fMaxGlobalVars;
    int fMaxTaskVars;
    bool fResources;
    bool fEvents;
    bool fArrays;
    bool fRestrictedMath;
    bool fSubParams;
    bool fHasExtendedMathOps;

    bool SourceWritable(int source) const;
    int GetChunkLimit(RCX_ChunkType type) const;
};


extern const RCX_Target *getTarget(RCX_TargetType type);


#endif
