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
