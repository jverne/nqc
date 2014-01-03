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
 * Portions created by David Baum are Copyright (C) 1998 David Baum.
 * All Rights Reserved.
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */

#include "IfStmt.h"
#include "Bytecode.h"

IfStmt::IfStmt(Expr *c, Stmt *s1, Stmt *s2) :
	BinaryStmt(s1, s2), fCondition(c)
{
}


IfStmt::~IfStmt()
{
	delete fCondition;
}


void IfStmt::EmitActual(Bytecode &b)
{
	int value;

	if (fCondition->Evaluate(value))
	{
		if (value && (!GetSecondary() || !GetSecondary()->GetMustEmit()))
		{
			GetPrimary()->Emit(b);
			return;
		}
		else if (!GetPrimary()->GetMustEmit())
		{
			if (GetSecondary()) GetSecondary()->Emit(b);
			return;
		}
	}

	if (GetSecondary())
		EmitIfElse(b);
	else
		EmitIf(b);
}


void IfStmt::EmitIf(Bytecode &b)
{
	int label;

	label = b.NewLabel();
	fCondition->EmitBranch(b, label, false);
	GetPrimary()->Emit(b);
	b.SetLabel(label);
}


void IfStmt::EmitIfElse(Bytecode &b)
{
	int testLabel = b.NewLabel();
	int outLabel= b.NewLabel();

	fCondition->EmitBranch(b, testLabel, false);

	// generate A and jump
	GetPrimary()->Emit(b);
	b.AddJump(outLabel);
	b.SetLabel(testLabel);

	// generate B
	GetSecondary()->Emit(b);
	b.SetLabel(outLabel);
}


Stmt* IfStmt::CloneActual(Mapping *b) const
{
	return new IfStmt(
		fCondition->Clone(b),
		GetPrimary()->Clone(b),
		GetSecondary() ? GetSecondary()->Clone(b) : 0);
}
