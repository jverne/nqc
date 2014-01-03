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

#include "RepeatStmt.h"
#include "Bytecode.h"
#include "RCX_Cmd.h"
#include "RCX_Target.h"
#include "Expr.h"
#include "Error.h"
#include "RCX_Constants.h"

#define REPEAT_MASK	(TYPEMASK(kRCX_VariableType) + \
					TYPEMASK(kRCX_ConstantType) + \
					TYPEMASK(kRCX_RandomType))


bool RepeatStmt::sRCXLoopInUse = false;

RepeatStmt::RepeatStmt(Expr *c, Stmt *s) :
	ChainStmt(s)
{
	fCount = c;
}


RepeatStmt::~RepeatStmt()
{
	delete fCount;
}


void RepeatStmt::EmitActual(Bytecode &b)
{
	RCX_TargetType tt = b.GetTarget()->fType;

	if (tt == kRCX_RCXTarget || tt == kRCX_CMTarget)
	{
		// see if loop is candidate for loop counter
		int n;
		bool bCountEval = fCount->Evaluate(n);

		if (bCountEval &&
			n >= 0 &&
			n < 256 &&
			!sRCXLoopInUse)
		{
			// mark the loop counter in use, then emit code
			sRCXLoopInUse = true;
			EmitRCXLoop(b);

			// loop counter is now available for future use
			sRCXLoopInUse = false;
		}
		else
		{
			// use a temp variable rather than the loop counter
			EmitRCXVar(b);
		}
	}
        else if (tt == kRCX_SwanTarget)
        {
		// Swan can't use EmitDecJump because if uses global variables
                EmitRCXVar(b);
        }
        else
	{
		EmitDecJump(b);
	}
}


/*
 * EmitRCXLoop(Bytecode &b) - emit code using the RCX loop counter.
 *   Since there is only one loop counter per task/sub, this cannot
 *   be used for nested repeats
 */

void RepeatStmt::EmitRCXLoop(Bytecode &b)
{
	/*
			set loop count
		cPos:
			check loop
			body
			jump -> cPos
	*/

	RCX_Cmd cmd;
	RCX_Value ea;

	int cLabel = b.PushFlow(Bytecode::kContinueFlow);
	int bLabel = b.PushFlow(Bytecode::kBreakFlow);

	ea = fCount->EmitConstrained(b, REPEAT_MASK);
	cmd.MakeSetLoop(ea);
	b.Add(cmd);
	b.ReleaseTempEA(ea);

	b.SetLabel(cLabel);

	cmd.MakeCheckLoop(0);
	b.Add(cmd);
	b.AddFixup(Bytecode::kSimpleLongFixup, bLabel, 1, kRCX_SCheckLoopOp);

	GetBody()->Emit(b);
	b.AddJump(cLabel);

	b.SetLabel(bLabel);
	b.PopFlow(Bytecode::kContinueFlow);
	b.PopFlow(Bytecode::kBreakFlow);
}


/*
 * EmitRCXVar(Bytecode &b) - emit using a temp variable and decrementing
 * 	it manually.
 */

void RepeatStmt::EmitRCXVar(Bytecode &b)
{
	/*
			setvar to loop count
		cPos:
			dec var
			check var
			body
			jump -> cPos
	*/

	RCX_Cmd cmd;
	int var;

	int cLabel = b.PushFlow(Bytecode::kContinueFlow);
	int bLabel = b.PushFlow(Bytecode::kBreakFlow);

	// allocate the loop variable
	var = EmitCountToTemp(b);

	// emit the decrement/check
	b.SetLabel(cLabel);
	b.AddTest(RCX_VALUE(kRCX_ConstantType, 0), kRCX_GreaterOrEqual,
		RCX_VALUE(kRCX_VariableType, var), bLabel);
	cmd.MakeVar(kRCX_SubVar, var, RCX_VALUE(kRCX_ConstantType, 1));
	b.Add(cmd);

	// body and jump back to check
	GetBody()->Emit(b);
	b.AddJump(cLabel);

	// done with temp and loop context
	b.GetVarAllocator().Release(var);

	b.SetLabel(bLabel);
	b.PopFlow(Bytecode::kContinueFlow);
	b.PopFlow(Bytecode::kBreakFlow);
}


/*
 * EmitDecJump(Bytecode &b) - emit using the decvjnl instruction
 */

void RepeatStmt::EmitDecJump(Bytecode &b)
{
	/*
			setvar to loop count
		cPos:
			check var
			body
			jump -> cPos
	*/

	RCX_Cmd cmd;
	int var;

	int cLabel = b.PushFlow(Bytecode::kContinueFlow);
	int bLabel = b.PushFlow(Bytecode::kBreakFlow);

	var = EmitCountToTemp(b);

	b.SetLabel(cLabel);

	cmd.Set(kRCX_DecVarJmpLTZeroOp, var, 0, 0);
	b.Add(cmd);

	b.AddFixup(Bytecode::kSignBitLongFixup, bLabel, 2, kRCX_SDecVarJmpLTZeroOp);

	GetBody()->Emit(b);
	b.AddJump(cLabel);

	b.GetVarAllocator().Release(var);

	b.SetLabel(bLabel);
	b.PopFlow(Bytecode::kContinueFlow);
	b.PopFlow(Bytecode::kBreakFlow);
}


int RepeatStmt::EmitCountToTemp(Bytecode &b)
{
	int var = b.GetTempVar(true);
	if (var == kIllegalVar)
		Error(kErr_NoMoreTemps).Raise(&fCount->GetLoc());
	else
		fCount->EmitTo(b, var);
	return var;
}


Stmt* RepeatStmt::CloneActual(Mapping *b) const
{
	return new RepeatStmt(fCount->Clone(b), GetBody()->Clone(b));
}


void RepeatStmt::GetExprs(vector<Expr *> &v) const
{
	v.push_back(fCount);
}
