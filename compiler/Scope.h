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

#ifndef __Scope_h
#define __Scope_h

#ifndef __PListS_h
#include "PListS.h"
#endif

#ifndef __Variable_h
#include "Variable.h"
#endif

#ifndef __AutoFree_h
#include "AutoFree.h"
#endif

#include <vector>

using std::vector;

class Symbol;


class Scope : public PLinkS<Scope>, public AutoFree
{
public:
	Scope();
	~Scope();

	bool	Define(const Symbol *name, int var, bool array, bool ptr, bool stack);
	int	Lookup(const Symbol *name, bool &array, bool &ptr, bool &stack);
	bool	Contains(const Symbol *name)	{ return Lookup1(name)!= kIllegalVar; }

private:
	struct ScopeData
	{
		ScopeData() : fName(0), fVar(kIllegalVar), fArray(0), fPtr(0), fStack(0) {}
		ScopeData(const Symbol *name, int var, bool a, bool ptr, bool stack) :
                  fName(name), fVar(var), fArray(a), fPtr(ptr), fStack(stack) {}

		const Symbol *	fName;
		int	fVar;
		bool	fArray;
                bool    fPtr;
                bool    fStack;
	};

	int	Lookup1(const Symbol *name, bool *array = 0, bool *ptr = 0, bool *stack = 0);

	vector<ScopeData>	fBindings;
};

#endif
