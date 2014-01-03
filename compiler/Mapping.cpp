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
#include "Mapping.h"
#include "Expr.h"

Mapping::Mapping()
{
}

Mapping::~Mapping()
{
	for(size_t i=0; i<fPairs.size(); ++i)
		delete fPairs[i].fValue;
}


void Mapping::Add(int var, Expr *e)
{
	fPairs.push_back(Pair(var, e));
}


const Expr* Mapping::Get(int var) const
{
	for(size_t i=0; i<fPairs.size(); ++i)
		if (fPairs[i].fVar == var) return fPairs[i].fValue;

	return 0;
}



int Mapping::GetMappedVar(int var) const
{
	const Expr *e = Get(var);
	if (!e) return var;

	return e->GetLValue();
}
