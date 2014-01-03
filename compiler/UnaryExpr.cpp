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
#include "UnaryExpr.h"
#include "parser.h"
#include "Bytecode.h"
#include "RCX_Cmd.h"
#include "Program.h"

static RCX_VarCode GetUnaryCode(int op);


static RCX_VarCode GetUnaryCode(int op)
{
        const RCX_Target *t = gProgram->GetTarget();
	switch(op)
	{
		case ABS:
			return kRCX_AbsVar;
		case SIGN:
			return kRCX_SgnVar;
                case '~':
                        if (t->fType == kRCX_SwanTarget)
                           return kRCX_NotVar;
                        else
                           return kRCX_IllegalVar;
		default:
			return kRCX_IllegalVar;
	}
}



Expr* UnaryExpr::Clone(Mapping *b) const
{
	return new UnaryExpr(fOp, Get(0)->Clone(b));
}


bool UnaryExpr::Evaluate(int &value) const
{
	if (!Get(0)->Evaluate(value)) return false;

	switch(fOp)
	{
		case '~':
			value = ~value;
			break;
		case ABS:
			if (value < 0) value = -value;
			break;
		case SIGN:
			if (value < 0)
				value = -1;
			else if (value > 0)
				value = 1;
			else
				value = 0;
			break;
		default:
			return false;

	}
	return true;
}


RCX_Value UnaryExpr::EmitAny_(Bytecode &b) const
{
	RCX_Cmd cmd;
	RCX_Value ea;
	int dst;

	ea = Get(0)->EmitMath(b);
	if (ea == kIllegalEA) return ea;

	if (b.IsTempEA(ea))
	{
		// operate in place
		dst = RCX_VALUE_DATA(ea);
	}
	else
	{
		// create a temp
		dst = GetTempVar(b);
		if (dst < 0)
			return kIllegalEA;
	}

	cmd.MakeVar(GetUnaryCode(fOp), dst, ea);
	b.Add(cmd);

	// return the ea
	return RCX_VALUE(kRCX_VariableType, dst);
}


bool UnaryExpr::EmitTo_(Bytecode &b, int dst) const
{
	RCX_Value ea;
	RCX_Cmd cmd;

	ea = Get(0)->EmitMath(b);
	if (ea == kIllegalEA) return false;

	cmd.MakeVar(GetUnaryCode(fOp), dst, ea);
	b.Add(cmd);
	b.ReleaseTempEA(ea);
	return true;
}


bool UnaryExpr::EmitSide_(Bytecode &b) const
{
	return Get(0)->EmitSide(b);
}


bool UnaryExpr::NeedsConstant(int op)
{
	return GetUnaryCode(op) == kRCX_IllegalVar;
}

