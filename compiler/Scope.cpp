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

#include "Scope.h"
#include "Variable.h"

Scope::Scope()
{
}

Scope::~Scope()
{
}


bool Scope::Define(const Symbol *name, int var, bool array, bool ptr, bool stack)
{
	if (Contains(name))
		return false;

	fBindings.push_back(ScopeData(name, var, array, ptr, stack));
	return true;
}


int Scope::Lookup(const Symbol *name, bool &array, bool &ptr, bool &stack)
{
	Scope *s;

	for(s=this; s; s=s->GetNext())
	{
		int var = s->Lookup1(name, &array, &ptr, &stack);
		if (var != kIllegalVar) return var;
	}

	return kIllegalVar;
}


int	Scope::Lookup1(const Symbol *name, bool *array, bool *ptr, bool *stack)
{
	for(size_t i=0; i<fBindings.size(); i++)
	{
		if (fBindings[i].fName == name)
		{
			if (array) *array = fBindings[i].fArray;
                        if (ptr) *ptr = fBindings[i].fPtr;
                        if (stack) *stack = fBindings[i].fStack;
			return fBindings[i].fVar;
		}
	}

	return kIllegalVar;
}
