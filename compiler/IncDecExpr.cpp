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

#include "IncDecExpr.h"
#include "RCX_Cmd.h"
#include "Bytecode.h"
#include "Mapping.h"

IncDecExpr::IncDecExpr(int var, bool inc, bool pre, const LexLocation &loc) :
	Expr(loc),
	fVar(var),
	fInc(inc),
	fPre(pre)
{
}


IncDecExpr::~IncDecExpr()
{
}


Expr* IncDecExpr::Clone(Mapping *m) const
{
	int newVar = m ? m->GetMappedVar(fVar) : fVar;
	return new IncDecExpr(newVar, fInc, fPre, GetLoc());
}


bool IncDecExpr::Contains(int var) const
{
	return var == fVar;
}


RCX_Value IncDecExpr::EmitAny_(Bytecode &b) const
{
	RCX_Value src;

	src = RCX_VALUE(kRCX_VariableType, fVar);

	if (fPre)
	{
		(void)EmitSide_(b);
		return src;
	}
	else
	{
		int var = GetTempVar(b);
		if (var == kIllegalVar) return kIllegalEA;

		b.AddMove(var, src);
		(void)EmitSide_(b);

		return RCX_VALUE(kRCX_VariableType, var);
	}
}



bool IncDecExpr::EmitTo_(Bytecode &b, int dst) const
{
	if (fPre)
	{
		EmitSide_(b);
		if (dst != fVar)
			b.AddMove(dst, fVar);
	}
	else
	{
		if (dst!=fVar)
		{
			b.AddMove(dst, fVar);
			EmitSide_(b);
		}
	}

	return true;
}


bool IncDecExpr::EmitSide_(Bytecode &b) const
{
	RCX_Cmd cmd;

	cmd.MakeVar(fInc ? kRCX_AddVar : kRCX_SubVar,
				fVar,
				RCX_VALUE(kRCX_ConstantType, 1));
	b.Add(cmd);
	return true;
}


void IncDecExpr::Translate(int from, int to)
{
	if (fVar == from)
		fVar = to;
}

