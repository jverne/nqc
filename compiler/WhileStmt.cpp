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

#include "WhileStmt.h"
#include "Bytecode.h"

WhileStmt::WhileStmt(Expr *e, Stmt *s) :
	ChainStmt(s)
{
	fCondition = e;
}


WhileStmt::~WhileStmt()
{
	delete fCondition;
}



void WhileStmt::EmitActual(Bytecode &b)
{
	bool optimized = false;
	int startLabel;
	int value;

	int cLabel = b.PushFlow(Bytecode::kContinueFlow);
	int bLabel = b.PushFlow(Bytecode::kBreakFlow);

	if (fCondition->Evaluate(value))
	{
		if (value)
		{
			/*
				cPos:
					body
					jump -> 1
			*/
			b.SetLabel(cLabel);
			GetBody()->Emit(b);
			b.AddJump(cLabel);
			optimized = true;
		}
		else if (!GetBody()->GetMustEmit())
		{
			// don't need to emit anything
			optimized = true;
		}
	}

	if (!optimized)
	{
		/* 		jump -> cPos
			startLabel:
				body
			cPos:
				test C -> startLabel
		*/

		int initialPosition = b.GetLength();
		b.AddJump(cLabel);
		startLabel = b.NewLabel();
		GetBody()->Emit(b);

		// if no body code has been emitted, then remove the
		// initial jump
		if (b.GetLabelPosition(startLabel) == b.GetLength()) {
			b.Truncate(initialPosition);
			b.SetLabel(startLabel);
		}

		b.SetLabel(cLabel);
		fCondition->EmitBranch(b, startLabel, true);
	}

	b.SetLabel(bLabel);
	b.PopFlow(Bytecode::kContinueFlow);
	b.PopFlow(Bytecode::kBreakFlow);
}


Stmt* WhileStmt::CloneActual(Mapping *b) const
{
	return new WhileStmt(fCondition->Clone(b), GetBody()->Clone(b));
}
