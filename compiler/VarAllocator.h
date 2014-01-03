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


#ifndef __VarAllocator_h
#define __VarAllocator_h

class VarAllocator
{
public:
	// allocation modes
	enum
	{
		kGlobalMode,
		kTaskMode,
		kSingleSubMode,
		kMultiSubMode
	};

		VarAllocator(int maxGlobals, int maxTaskVars);
		~VarAllocator();

	bool	Reserve(int v);	// permenant reservation

	int	Allocate(bool temp, bool canUseLocals, int count);
	void	Release(int v);

	bool	IsTemp(int v) const;
	void	ReleaseTemp(int v);

	void	Begin(int mode);
	int	End();

	bool	CheckLocalMask(int localMask) const;

private:
	enum VarState {
		// these states are available for variable allocation
		kFree = 0,
		kDirty,

		// states from here up are considered "used"
		kUsed,
		kTemp,
		kReserved
	};

	bool	IsLegal(int v) const { return (v >= 0 && v < fMaxVars); }
	int	FindUnused(int start, int end, int count);
	bool	CheckAvailable(int first, int count) const;

	int		fMode;
	int		fMaxVars;
	int		fLocalStart;
	VarState*	fStates;
};

#endif
