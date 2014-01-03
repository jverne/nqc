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
#include "DeclareStmt.h"
#include "AssignStmt.h"
#include "ScopeStmt.h"
#include "Variable.h"
#include "Bytecode.h"
#include "Error.h"
#include "AtomExpr.h"
#include "Symbol.h"


DeclareStmt::DeclareStmt(const Symbol *name, int var, const struct LexLocation &loc, int count, bool ptr, bool stack) :
	ChainStmt(),
	fName(name),
	fVar(var),
	fScope(0),
	fLoc(loc),
	fCount(count),
        fPtr(ptr),
        fStack(stack)
{
}


DeclareStmt::~DeclareStmt()
{
}



void DeclareStmt::SetInitialValue(Expr *e)
{
	SetBody(new AssignStmt(new AtomExpr(kRCX_VariableType, fVar, e->GetLoc()), e));
}


void DeclareStmt::EmitActual(Bytecode &b)
{
	if (fVar & kVirtualVarBase && fScope)
	{
		int physical = b.GetVarAllocator().Allocate(false, true, fCount);
		if (physical == kIllegalVar)
		{
			Error(kErr_NoMoreVars).Raise(&fLoc);
		}

		fScope->RemapVar(fVar, physical, fCount);

		// add symbolic information
		if (fName)
			b.AddVariableName(physical, fName->GetKey());
	}

	Stmt *body = GetBody();
	if (body) body->Emit(b);
}


Stmt* DeclareStmt::CloneActual(Mapping *b) const
{
	DeclareStmt *s = new DeclareStmt(fName, fVar, fLoc, fCount, fPtr, fStack);
	if (GetBody())
		s->SetBody(GetBody()->Clone(b));

	return s;
}


bool DeclareStmt::Binder::operator()(Stmt *s)
{
	if (ScopeStmt *ss = dynamic_cast<ScopeStmt*>(s))
	{
		// recurse into scopes with new Binder, then return false
		Binder b(ss);
		Apply(ss->GetBody(),b);
		return false;
	}

	// bind declarations to scopes
	if (DeclareStmt *ds = dynamic_cast<DeclareStmt*>(s))
	{
		ds->fScope = fScope;
	}

	return true;
}
