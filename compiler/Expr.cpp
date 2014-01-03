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

#include "Expr.h"
#include "RCX_Cmd.h"
#include "Bytecode.h"
#include "Error.h"
#include "RCX_Target.h"

#define MAX_INT	65535
#define MIN_INT	-32768

// suitable sources for math operations (if target has constrained math)
#define MATH_MASK	(TYPEMASK(kRCX_VariableType) + \
					TYPEMASK(kRCX_ConstantType))

static bool IsTypeInMask(long mask, RCX_ValueType type);


Expr::~Expr()
{
}


RCX_Value Expr::EmitAny(Bytecode &b) const
{
	int v;

	if (Evaluate(v))
	{
		if (v < MIN_INT || v > MAX_INT)
			Error(kErr_NumberRange).Raise(&fLoc);

		return RCX_VALUE(kRCX_ConstantType, v);
	}
	else
		return EmitAny_(b);
}


bool Expr::EmitTo(Bytecode &b, int dst) const
{
	int v;

	if (Evaluate(v))
	{
		b.AddMove(dst, RCX_VALUE(kRCX_ConstantType, v));
		return true;
	}
	else
		return EmitTo_(b, dst);
}


RCX_Value Expr::EmitConstrained(Bytecode &b, long mask, bool canUseLocals) const
{
	/*
	 * There's one case that still isn't optimized...if locals cannot be used,
	 * but EmitAny() wants a temp (such as for a math operation), then it is
	 * possible for a local temp to be allocated, which just forces a move to
	 * a global temp later on.  It would be better if EmitAny() could be told
	 * not to use locals, but this could involve a lot of tweaking.  A possible
	 * hack is to put the VarAllocator into kGlobal mode prior to an AsmField
	 * calling EmitConstrained, then restoring the mode later on.
	 */
	RCX_Value ea = EmitAny(b);
	if (ea == kIllegalEA) return ea;

	RCX_ValueType type = RCX_VALUE_TYPE(ea);

	// check ea was allowed
	if (IsTypeInMask(mask, type) && (canUseLocals || !b.IsLocalEA(ea)))
	{
			return ea;
	}

	// must move to another ea

	// if registers are not valid, then return illegal EA
	if (!IsTypeInMask(mask, kRCX_VariableType))
	{
		Error(kErr_BadExpression).Raise(&GetLoc());
		return kIllegalEA;
	}

	// move into temp register
	int dst = GetTempVar(b, canUseLocals);
	if (dst == kIllegalVar) return kIllegalEA;

	b.AddMove(dst, ea);

	// when using arrays, there's a special case when the ea is
	// temp but not a variable.  Another case arises if the original
	// emit was to a temp local, but they aren't allowed.
	// call ReleaseTempEA just in case
	b.ReleaseTempEA(ea);

	ea = RCX_VALUE(kRCX_VariableType, dst);

	return ea;
}

bool IsTypeInMask(long mask, RCX_ValueType type)
{
	// special case, when mask==0 all types are allowed
	if (!mask) return true;

	return (1L << type) & mask;
}

RCX_Value Expr::EmitMath(Bytecode &b) const
{
	// emit expression to an ea that is suitable for math operations
	if (b.GetTarget()->fRestrictedMath)
		return EmitConstrained(b, MATH_MASK);
	else
		return EmitAny(b);
}


bool Expr::EmitSide(Bytecode &b) const
{
	return EmitSide_(b);
}


bool Expr::EmitBranch(Bytecode &b, int label, bool condition) const
{
	int v;

	if (Evaluate(v))
	{
		if ((v && condition) ||
			(!v && !condition))
		{
			b.AddJump(label);
		}

		return true;
	}

	return EmitBranch_(b, label, condition);
}


RCX_Value Expr::GetStaticEA() const
{
	int v;

	if (Evaluate(v))
	{
		return RCX_VALUE(kRCX_ConstantType, v);
	}

	return GetStaticEA_();
}


RCX_Value Expr::GetStaticEA_() const
{
	return kIllegalEA;
}


bool Expr::EmitTo_(Bytecode &b, int dst) const
{
	RCX_Value ea = EmitAny_(b);

	if (ea == kIllegalEA) return false;

	b.AddMove(dst, ea);
	b.ReleaseTempEA(ea);

	return true;
}


bool Expr::EmitSide_(Bytecode &b) const
{
	vector<Expr *> v;
	GetExprs(v);
	for(int i=0; i<(int)v.size(); ++i)
		if (!v[i]->EmitSide(b)) return false;

	return true;
}


bool Expr::EmitBranch_(Bytecode &b, int label, bool condition) const
{
	RCX_Value ea = EmitConstrained(b, TEST_MASK);
	if (ea==kIllegalEA) return false;

	// compare ea to 0 and branch
	b.AddTest(
		RCX_VALUE(kRCX_ConstantType,0),
		condition ? kRCX_NotEqualTo : kRCX_EqualTo,
		ea, label);

	b.ReleaseTempEA(ea);

	return true;
}


RCX_Value Expr::EmitBoolAny(Bytecode &b) const
{
	int dst = GetTempVar(b);
	if (dst < 0)
		return kIllegalEA;

	RCX_Value dstEA = RCX_VALUE(kRCX_VariableType, dst);

	if (!EmitBoolTo(b, dst))
	{
		b.ReleaseTempEA(dstEA);
		return kIllegalEA;
	}

	return dstEA;
}


bool Expr::EmitBoolTo(Bytecode &b, int dst) const
{
	int doneLabel = b.NewLabel();

	if (Contains(dst))
	{
		/*
		 *   if condition then goto 1
		 *   dst = 0
		 *   goto done
		 * 1:
		 *   dst = 1
		 * done:
		 */
		int setLabel = b.NewLabel();
		EmitBranch(b, setLabel, true);
		b.AddMove(dst, RCX_VALUE(kRCX_ConstantType, 0));
		b.AddJump(doneLabel);
		b.SetLabel(setLabel);
		b.AddMove(dst, RCX_VALUE(kRCX_ConstantType, 1));
	}
	else
	{
		/*   dst = 0
		 *   if !condition then goto done
		 *   dst = 1
		 * done:
		 */
		b.AddMove(dst, RCX_VALUE(kRCX_ConstantType, 0));
		EmitBranch(b, doneLabel, false);
		b.AddMove(dst, RCX_VALUE(kRCX_ConstantType, 1));
	}

	b.SetLabel(doneLabel);
	return true;
}


int Expr::GetTempVar(Bytecode &b, bool canUseLocals) const
{
	int var = b.GetTempVar(canUseLocals);

	if (var == kIllegalVar)
	{
		Error(kErr_NoMoreTemps).Raise(&GetLoc());
	}

	return var;
}
