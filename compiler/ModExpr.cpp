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

#include "ModExpr.h"
#include "Bytecode.h"
#include "RCX_Cmd.h"

#define NON_VOLATILE_MASK	(TYPEMASK(kRCX_VariableType) + \
							 TYPEMASK(kRCX_ConstantType))


Expr* ModExpr::Clone(Mapping *b) const
{
	return new ModExpr(Get(0)->Clone(b), Get(1)->Clone(b));
}


bool ModExpr::Evaluate(int &value) const
{
	int v1, v2;

	if (!Get(0)->Evaluate(v1)) return false;
	if (!Get(1)->Evaluate(v2)) return false;

	value = v1 % v2;

	return true;
}


RCX_Value ModExpr::EmitAny_(Bytecode &b) const
{
	int dst;

	dst = GetTempVar(b);
	if (dst < 0)
		return kIllegalEA;

	RCX_Value ea  = RCX_VALUE(kRCX_VariableType, dst);

	if (EmitCalculation(b, dst))
	{
		return ea;
	}
	else
	{
		b.ReleaseTempEA(ea);
		return kIllegalEA;
	}
}


bool ModExpr::EmitTo_(Bytecode &b, int dst) const
{
	if (Get(1)->Contains(dst) || Get(0)->Contains(dst))
	{
		RCX_Value ea;

		ea = EmitAny_(b);
		if (ea==kIllegalEA) return false;

		b.AddMove(dst, ea);
		b.ReleaseTempEA(ea);
		return true;
	}
	else
	{
		return EmitCalculation(b, dst);
	}
}


bool ModExpr::EmitCalculation(Bytecode &b, int dst) const
{
	RCX_Value m, n;
	RCX_Cmd cmd;

	m = Get(0)->EmitConstrained(b, NON_VOLATILE_MASK);
	if (m == kIllegalEA) return false;

	n = Get(1)->EmitConstrained(b, NON_VOLATILE_MASK);
	if (n== kIllegalEA)
	{
		b.ReleaseTempEA(m);
		return false;
	}

	// dst = m
	b.AddMove(dst, m);

	// dst /= n
	cmd.MakeVar(kRCX_DivVar ,dst, n);
	b.Add(cmd);

	// dst *= n
	cmd.MakeVar(kRCX_MulVar, dst, n);
	b.Add(cmd);

	// dst -= m
	cmd.MakeVar(kRCX_SubVar, dst, m);
	b.Add(cmd);

	// dst *= -1
	cmd.MakeVar(kRCX_MulVar, dst, RCX_VALUE(kRCX_ConstantType, -1));
	b.Add(cmd);

	b.ReleaseTempEA(m);
	b.ReleaseTempEA(n);

	return true	;
}

