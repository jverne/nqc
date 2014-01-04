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
#include "DoStmt.h"
#include "Bytecode.h"
#include "JumpStmt.h"

DoStmt::DoStmt(Expr *c, Stmt *s) :
	ChainStmt(s)
{
	fCondition = c;
}


DoStmt::~DoStmt()
{
	delete fCondition;
}


void DoStmt::EmitActual(Bytecode &b)
{
	/*
	 sLabel:
	 	fBody
	 cLabel:
	 	test -> sLabel
	 bLabel:
	*/

	int cLabel = b.PushFlow(Bytecode::kContinueFlow);
	int bLabel = b.PushFlow(Bytecode::kBreakFlow);
	int sLabel = b.NewLabel();

	GetBody()->Emit(b);
	b.SetLabel(cLabel);

	fCondition->EmitBranch(b, sLabel, true);

	b.SetLabel(bLabel);
	b.PopFlow(Bytecode::kContinueFlow);
	b.PopFlow(Bytecode::kBreakFlow);
}

Stmt* DoStmt::CloneActual(Mapping *b) const
{
	return new DoStmt(fCondition->Clone(b), GetBody()->Clone(b));
}
