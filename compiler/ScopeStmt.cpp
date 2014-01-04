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
#include "ScopeStmt.h"
#include "VarTranslator.h"
#include "Bytecode.h"

ScopeStmt::ScopeStmt(Stmt *s) :
	ChainStmt(s)
{
}


ScopeStmt::~ScopeStmt()
{
}


void ScopeStmt::EmitActual(Bytecode &b)
{
	GetBody()->EmitActual(b);

	// release vars in the reverse order they were allocated
	for(int i=fVariables.size()-1; i>=0; --i)
		b.GetVarAllocator().Release(fVariables[i]);
}


Stmt* ScopeStmt::CloneActual(Mapping *b) const
{
	return new ScopeStmt(GetBody()->Clone(b));
}


void ScopeStmt::RemapVar(int from, int to, int count)
{
	VarTranslator vt(from, to);
	Apply(this, vt);
	while(count--)
		fVariables.push_back(to++);
}
