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

#include "BinaryExpr.h"
#include "parser.h"
#include "Bytecode.h"
#include "RCX_Cmd.h"
#include "Program.h"


static RCX_VarCode GetBinaryCode(int op);

static RCX_VarCode GetBinaryCode(int op)
{
        const RCX_Target *t = gProgram->GetTarget();
	switch(op)
	{
		case '+':
			return kRCX_AddVar;
		case '-':
			return kRCX_SubVar;
		case '*':
			return kRCX_MulVar;
		case '/':
			return kRCX_DivVar;
		case '&':
			return kRCX_AndVar;
		case '|':
			return kRCX_OrVar;
		case '%':
			if (t->fType == kRCX_SwanTarget)
				return kRCX_ModVar;
			else
				return kRCX_IllegalVar;
		case LEFT:
			if (t->fType == kRCX_SwanTarget)
				return kRCX_ShlVar;
			else
				return kRCX_IllegalVar;
		case RIGHT:
			if (t->fType == kRCX_SwanTarget)
				return kRCX_ShrVar;
			else
				return kRCX_IllegalVar;
		case '^':
			if (t->fType == kRCX_SwanTarget)
				return kRCX_XOrVar;
			else
				return kRCX_IllegalVar;
		default:
			return kRCX_IllegalVar;
	}
}


BinaryExpr::BinaryExpr(Expr *lhs, int op, Expr *rhs)
	: NodeExpr(lhs, rhs), fOp(op)
{
}



Expr* BinaryExpr::Clone(Mapping *b) const
{
	return new BinaryExpr(Get(0)->Clone(b), fOp, Get(1)->Clone(b));
}


bool BinaryExpr::Evaluate(int &value) const
{
	int v1, v2;

	if (!Get(0)->Evaluate(v1)) return false;
	if (!Get(1)->Evaluate(v2)) return false;

	switch(fOp)
	{
		case '+':
			value = v1 + v2;
			break;
		case '-':
			value = v1 - v2;
			break;
		case '*':
			value = v1 * v2;
			break;
		case '/':
			value = v1 / v2;
			break;
		case '&':
			value = v1 & v2;
			break;
		case '|':
			value = v1 | v2;
			break;
		case '%':
			value = v1 % v2;
			break;
		case LEFT:
			value =	v1 << v2;
			break;
		case RIGHT:
			value = v1 >> v2;
			break;
		case '^':
			value = v1 ^ v2;
			break;
		default:
			return false;

	}
	return true;
}


RCX_Value BinaryExpr::EmitAny_(Bytecode &b) const
{
	RCX_Cmd cmd;
	RCX_Value ea;
	RCX_Value dstEA;
	int dst;
	RCX_VarCode code;

	ea = Get(0)->EmitAny(b);
	if (ea == kIllegalEA) return ea;

	if (b.IsTempEA(ea))
		dst = RCX_VALUE_DATA(ea);
	else
	{
		dst = GetTempVar(b);
		if (dst < 0)
			return kIllegalEA;

		b.AddMove(dst, ea);
	}

	code = GetBinaryCode(fOp);
	dstEA = RCX_VALUE(kRCX_VariableType, dst);
	ea = Get(1)->EmitMath(b);
	if (ea == kIllegalEA)
	{
		b.ReleaseTempEA(dstEA);
		return ea;
	}

	cmd.MakeVar(code, dst, ea);
	b.Add(cmd);
	b.ReleaseTempEA(ea);

	return dstEA;
}


bool BinaryExpr::EmitTo_(Bytecode &b, int dst) const
{
	if (Get(1)->Contains(dst))
		return Expr::EmitTo_(b, dst);
	else
	{
		Get(0)->EmitTo(b, dst);

		RCX_Cmd cmd;
		RCX_VarCode code = GetBinaryCode(fOp);
		RCX_Value ea = Get(1)->EmitMath(b);
		if (ea == kIllegalEA) return false;

		cmd.MakeVar(code, dst, ea);
		b.Add(cmd);
		b.ReleaseTempEA(ea);
		return true;
	}
}

bool BinaryExpr::NeedsConstant(int op)
{
	return GetBinaryCode(op) == kRCX_IllegalVar;
}
