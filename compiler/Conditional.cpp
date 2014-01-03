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
#include "Conditional.h"

Conditional::Conditional()
{
	Level *l = new Level;

	l->fState = Level::kActive;
	l->fElseAllowed = false;
	l->fTopLevel = true;

	fLevels.InsertHead(l);
}


Conditional::~Conditional()
{
	Level *l;
	while((l = fLevels.RemoveHead()) != nil)
		delete l;
}


bool Conditional::Elif(bool b)
{
	Level *l = GetLevel();

	if (!l->fElseAllowed) return false;

	if (l->fState == Level::kPending)
	{
		l->fState = (b ? Level::kActive : Level::kPending);
	}
	else
		l->fState = Level::kInactive;

	return true;
}


void Conditional::If(bool b)
{
	Level *l = new Level;
	Level::State state;

	if (IsActive())
	{
		if (b)
			state = Level::kActive;
		else
			state = Level::kPending;
	}
	else
		state = Level::kInactive;

	l->fState = state;
	l->fElseAllowed = true;
	l->fTopLevel = false;

	fLevels.InsertHead(l);
}


bool Conditional::Else()
{
	Level *l = GetLevel();

	if (!l->fElseAllowed) return false;

	l->fElseAllowed = false;
	switch(l->fState)
	{
		case Level::kActive:
			l->fState = Level::kInactive;
			break;
		case Level::kPending:
			l->fState = Level::kActive;
			break;
		default:
			// no change for inactive level
			break;
	}

	return true;
}


bool Conditional::Endif()
{
	Level *l;

	if (IsTopLevel()) return false;

	l = fLevels.RemoveHead();
	delete l;

	return true;
}
