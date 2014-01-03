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
