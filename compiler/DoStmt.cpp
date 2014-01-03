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
