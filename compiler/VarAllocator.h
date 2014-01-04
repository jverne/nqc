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
