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
