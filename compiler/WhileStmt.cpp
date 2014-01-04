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
