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
#include "ShiftExpr.h"
#include "Bytecode.h"
#include "RCX_Cmd.h"


Expr* ShiftExpr::Clone(Mapping *b) const
{
	return new ShiftExpr(Get(0)->Clone(b), Get(1)->Clone(b), fDirection);
}


bool ShiftExpr::Evaluate(int &value) const
{
	int v1, v2;

	if (!Get(0)->Evaluate(v1)) return false;
	if (!Get(1)->Evaluate(v2)) return false;

	if (fDirection == kRight)
		value = v1 >> v2;
	else
		value = v1 << v2;

	return true;
}


RCX_Value ShiftExpr::EmitAny_(Bytecode &b) const
{
	int dst;

	dst = GetTempVar(b);
	if (dst < 0)
		return kIllegalEA;

	if (!EmitTo_(b, dst))
		return kIllegalEA;

	return RCX_VALUE(kRCX_VariableType, dst);
}


bool ShiftExpr::EmitTo_(Bytecode &b, int dst) const
{
	int shiftCount;

	// Get(1) *should* be a constant, but we check anyway
	if (!Get(1)->Evaluate(shiftCount)) return false;

	// compute shift value
	if (shiftCount > 15)
	{
		// too big...just zero out dst
		b.AddMove(dst, RCX_VALUE(kRCX_ConstantType, 0));
		return true;
	}
	else if (shiftCount < 0)
	{
		shiftCount = 0;
	}

	// factor is 2 ^ shiftCount
	int factor = 1 << shiftCount;


	// evaluate Get(0) into dst
	if (!Get(0)->EmitTo(b,dst)) return false;

	RCX_Cmd cmd;
	if (fDirection==kRight)
	{
		// divide by factor

		// branch if positive
		int posLabel = b.NewLabel();
		b.AddTest(RCX_VALUE(kRCX_ConstantType, 0), kRCX_LessOrEqual, RCX_VALUE(kRCX_VariableType, dst), posLabel);

		// code for negative operand
		cmd.MakeVar( kRCX_AndVar, dst, RCX_VALUE(kRCX_ConstantType, 0x7fff));
		b.Add(cmd);

		cmd.MakeVar( kRCX_DivVar, dst, RCX_VALUE(kRCX_ConstantType, factor));
		b.Add(cmd);

		cmd.MakeVar( kRCX_OrVar, dst, RCX_VALUE(kRCX_ConstantType, 0x8000 >> shiftCount));
		b.Add(cmd);

		int doneLabel = b.NewLabel();
		b.AddJump(doneLabel);

		// code for positive operand
		b.SetLabel(posLabel);
		cmd.MakeVar( kRCX_DivVar, dst, RCX_VALUE(kRCX_ConstantType, factor));
		b.Add(cmd);

		b.SetLabel(doneLabel);
	}
	else
	{
		// multiply by factor
		cmd.MakeVar(kRCX_MulVar, dst, RCX_VALUE(kRCX_ConstantType, factor));
		b.Add(cmd);
	}

	return true;
}
