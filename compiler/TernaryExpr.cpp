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
#include "TernaryExpr.h"
#include "Bytecode.h"
#include "RCX_Cmd.h"


Expr* TernaryExpr::Clone(Mapping *b) const
{
	return new TernaryExpr(Get(0)->Clone(b), Get(1)->Clone(b), Get(2)->Clone(b));
}


bool TernaryExpr::Evaluate(int &value) const
{
	int test;
	const Expr *e;

	if (!Get(0)->Evaluate(test)) return false;

	e = test ? Get(1) : Get(2);

	if (!e->Evaluate(value)) return false;

	return true;
}



RCX_Value TernaryExpr::EmitAny_(Bytecode &b) const
{
	int test;

	if (Get(0)->Evaluate(test)) {
		if (test)
			return Get(1)->EmitAny(b);
		else
			return Get(2)->EmitAny(b);
	}

	int dst = GetTempVar(b);
	if (dst < 0) return kIllegalEA;

	EmitConditionTo(b, dst);

	return RCX_VALUE(kRCX_VariableType, dst);
}


bool TernaryExpr::EmitTo_(Bytecode &b, int dst) const
{
	int test;

	if (Get(0)->Evaluate(test)) {
		if (test)
			return Get(1)->EmitTo(b, dst);
		else
			return Get(2)->EmitTo(b, dst);
	}

	return EmitConditionTo(b, dst);
}


bool TernaryExpr::EmitConditionTo(Bytecode &b, int dst) const
{
	int elseLabel = b.NewLabel();
	int doneLabel = b.NewLabel();

	if (!Get(0)->EmitBranch(b, elseLabel, false)) return false;

	if (!Get(1)->EmitTo(b, dst)) return false;
	b.AddJump(doneLabel);

	b.SetLabel(elseLabel);
	if (!Get(2)->EmitTo(b, dst)) return false;
	b.SetLabel(doneLabel);

	return true;
}


