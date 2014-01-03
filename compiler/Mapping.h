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


#ifndef __Mapping_h
#define __Mapping_h

#ifndef __Variable_h
#include "Variable.h"
#endif

#include <vector>

using std::vector;

class Expr;

class Mapping
{
public:
				Mapping();
				~Mapping();

	void		Add(int var, Expr *value);
	const Expr*	Get(int var) const;

	int			GetMappedVar(int var) const;

private:
	struct Pair
	{
				Pair(int v, Expr *e) : fVar(v), fValue(e) {}

		int		fVar;
		Expr*	fValue;
	};

	vector<Pair>	fPairs;
};

#endif
