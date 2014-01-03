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
#ifndef __Conditional_h
#define __Conditional_h

#ifndef __PListS_h
#include "PListS.h"
#endif

class Conditional
{
public:
			Conditional();
			~Conditional();

	bool	IsActive()		{ return (GetLevel()->fState == Level::kActive) ? true : false; }
	bool	IsTopLevel()	{ return GetLevel()->fTopLevel; }

	void	If(bool b);
	bool	Elif(bool b);
	bool	Else();
	bool	Endif();

private:
	class Level : public PLinkS<Level>
	{
	public:
		// states for a level
		enum State
		{
			kInactive,
			kActive,
			kPending
		};

		State	fState;			// see above
		bool	fElseAllowed;	// true until else is done
		bool	fTopLevel;		// only true for the top level
	};

	Level*	GetLevel()		{ return fLevels.GetHead(); }

	PListSS<Level>	fLevels;
};


#endif
