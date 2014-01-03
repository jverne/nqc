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


