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
#include "ForStmt.h"
#include "Bytecode.h"

ForStmt::ForStmt(Stmt *init, Expr *c, Stmt *iterate, Stmt *body) :
	fInit(init),
	fCondition(c),
	fIterate(iterate),
	fBody(body)
{
	// note that fInit, fCondition, and fIterate are all optional and
	// may be NULL!

	// we need to keep child statments in a list
	fChildren.InsertHead(fBody);
	if (fIterate)
		fChildren.InsertHead(fIterate);
	if (fInit)
		fChildren.InsertHead(fInit);

	Adopt(init);
	Adopt(iterate);
	Adopt(body);
}


ForStmt::~ForStmt()
{
	delete fInit;
	delete fCondition;
	delete fIterate;
	delete fBody;
}


void ForStmt::EmitActual(Bytecode &b)
{
	/*
			init
		loop:
			test !condition -> break
			body
		continue:
			iterate
			jump -> loop
		break:
	*/

	if (fInit)
		fInit->Emit(b);


	int cLabel = b.PushFlow(Bytecode::kContinueFlow);
	int bLabel = b.PushFlow(Bytecode::kBreakFlow);

	int loopLabel = b.NewLabel();

	if (fCondition)
		fCondition->EmitBranch(b, bLabel, false);

	fBody->Emit(b);

	b.SetLabel(cLabel);
	if (fIterate)
		fIterate->Emit(b);
	b.AddJump(loopLabel);

	b.SetLabel(bLabel);
	b.PopFlow(Bytecode::kContinueFlow);
	b.PopFlow(Bytecode::kBreakFlow);
}


Stmt* ForStmt::CloneActual(Mapping *b) const
{
	return new ForStmt(
		fInit ? fInit->Clone(b) : 0,
		fCondition ? fCondition->Clone(b) : 0,
		fIterate ? fIterate->Clone(b) : 0,
		fBody->Clone(b));
}

