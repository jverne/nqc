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
