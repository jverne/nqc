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
 * Portions created by David Baum are Copyright (C) 2000 David Baum.
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
