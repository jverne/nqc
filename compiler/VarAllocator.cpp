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
 * Portions created by David Baum are Copyright (C) 2000 David Baum.
 * All Rights Reserved.
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */

/*
 * Allocation Strategy
 *
 * fMode			Allocation
 * ---------------------------
 * kGlobalMode		Low Global
 *
 * kTaskMode		High Local
 *					Low Global
 *
 * kSingleSubMode	Low Local
 *					Low Global
 *
 * kMultiSubMode	Low Local
 *
 *
 * In general local variables are prefered to globals.  The
 * only exception is for true global variables (kGlobalMode)
 * which are not allowed to use the local pool.
 *
 * In cases where a single task is known to be in execution
 * (kTaskMode or kSingleSubMode) allocation is allowed to
 * overflow from the local pool to the global pool.
 *
 * Vars allocated from the global pool may be re-used within the
 * same Begin()/End() session, but then become permanently
 * allocated after End().  In contrast, the local pool is refreshed
 * each time Begin() is called - in the case of tasks this is
 * justified by the fact that each task has its own local vars.
 * For subs, this is because the true variable allocation is
 * done within the calling task, and this is checked for conflicts
 * by CheckAvailable().
 *
 * This strategy ensures that subs always use the bottom-most
 * local vars (overflow to globals in the case of a sub called
 * by only 1 task).  Allocations for a task use up locals from
 * the high end, which means that the only time a call to a sub
 * will conflict with the presently allocated task vars is if
 * local space is exhausted.
 *
 */

#include "VarAllocator.h"
#include "Variable.h"

#ifdef DEBUG
#include <cstdio>
using std::printf;
//#define DEBUG_VARS
#endif

VarAllocator::VarAllocator(int maxGlobals, int maxTaskVars) :
	fMode(kGlobalMode),
	fMaxVars(maxGlobals + maxTaskVars),
	fLocalStart(maxGlobals)
{

	fStates = new VarState[fMaxVars];
	for(int i=0; i<fMaxVars; ++i)
		fStates[i] = kFree;
}


VarAllocator::~VarAllocator()
{
	delete [] fStates;
}


bool VarAllocator::Reserve(int v)
{
	if (!IsLegal(v)) return false;

	if (fStates[v] != kFree) return false;

	fStates[v] = kReserved;
	return true;
}


int VarAllocator::Allocate(bool temp, bool canUseLocals, int count)
{
	int v = kIllegalVar;

	switch(fMode)
	{
		case kGlobalMode:
			v = FindUnused(0, fLocalStart, count);
			break;
		case kTaskMode:
			if (canUseLocals)
				v = FindUnused(fMaxVars, fLocalStart, count);
			if (v==kIllegalVar)
				v = FindUnused(0, fLocalStart, count);
			break;
		case kSingleSubMode:
			if (canUseLocals)
				v = FindUnused(fLocalStart, fMaxVars, count);
			if (v==kIllegalVar)
				v = FindUnused(0, fLocalStart, count);
			break;
		case kMultiSubMode:
			if (canUseLocals)
				v = FindUnused(fLocalStart, fMaxVars, count);
			break;
	}

	if (v != kIllegalVar)
	{
		for(int i=0; i<count; ++i)
		{
			fStates[v + i] = temp ? kTemp : kUsed;
#ifdef DEBUG_VARS
			printf("Allocate var %d\n", v + i);
#endif
		}
	}

	return v;
}


void VarAllocator::Release(int v)
{
	if (IsLegal(v) && ((fStates[v] == kUsed) || fStates[v]==kTemp))
	{
#ifdef DEBUG_VARS
	printf("Release var %d\n", v);
#endif
		fStates[v] = kDirty;
	}
}


bool VarAllocator::IsTemp(int v) const
{
	return IsLegal(v) && fStates[v]==kTemp;
}


void VarAllocator::ReleaseTemp(int v)
{
	if (IsTemp(v)) Release(v);
}


void VarAllocator::Begin(int mode)
{
	fMode = mode;

	// always start with a clean set of locals
	for(int i=fLocalStart; i<fMaxVars; ++i) {
		if (fStates[i] != kReserved)
			fStates[i] = kFree;
	}
}


int VarAllocator::End()
{
	// at each switch, make any dirty globals vars unusable
	// no vars should be kTemp, but just in case reserve them as well
	for(int i=0; i<fLocalStart; ++i)
	{
		if ((fStates[i] == kDirty) || (fStates[i] == kTemp))
			fStates[i] = kUsed;
	}


	// now figure out the local mask
	int localMask = 0;
	for(int i=fLocalStart; i<fMaxVars; ++i)
	{
		if (fStates[i] != kFree && fStates[i] != kReserved)
			localMask |= 1 << (i-fLocalStart);
	}

	fMode = kGlobalMode;

	return localMask;
}


bool VarAllocator::CheckLocalMask(int localMask) const
{
	for(int i=fLocalStart; i<fMaxVars; ++i) {
		if (localMask & (1 << (i-fLocalStart))) {
			if (fStates[i] >= kUsed) return false;
		}
	}

	return true;
}


int VarAllocator::FindUnused(int start, int end, int count)
{
	if (start < end)
	{
		// search [start, end) from bottom
		end -= count;
		for(int i=start; i<=end; ++i)
			if (CheckAvailable(i, count)) return i;
	}
	else
	{
		// search [end, start) from top
		for(int i=start-count; i>=end; --i)
			if (CheckAvailable(i, count)) return i;
	}

	return kIllegalVar;
}


bool VarAllocator::CheckAvailable(int first, int count) const
{
	while(count--)
	{
		if (fStates[first++] >= kUsed) return false;
	}

	return true;
}
