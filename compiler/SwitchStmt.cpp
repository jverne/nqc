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
#include "SwitchStmt.h"
#include "Bytecode.h"
#include "CaseStmt.h"
#include "Error.h"

class SwitchCaseEmitter
{
public:
				SwitchCaseEmitter(Bytecode &b, SwitchState &s) : fBytecode(b), fSwitchState(s) {};
	bool		operator()(Stmt *s);

private:
	Bytecode&		fBytecode;
	SwitchState&	fSwitchState;
};


SwitchStmt::SwitchStmt(Expr *c, Stmt *s) :
	ChainStmt(s)
{
	fSelector = c;
}


SwitchStmt::~SwitchStmt()
{
	delete fSelector;
}


void SwitchStmt::EmitActual(Bytecode &b)
{
	/*
	 * Code should look like this
	 *
	 * evalutate selector into variable
	 * test case 1
	 * test case 2
	 * ...
	 * test case N
	 * jump to default
	 * body case A
	 * body case B
	 * ...
	 * body case M
	 *
	 *
	 * Note that cases will be tested in the order they appear,
	 * with the exception of the default case, which is never tested.
	 * Code is emited for the body in the exact order it appears -
	 * default case may be in the middle!
	 */

	RCX_Value selector;
	int defaultLabel = b.NewLabel();

	// emit code to evaluate the selector
	selector = fSelector->EmitConstrained(b,TYPEMASK(kRCX_VariableType));
	if (selector == Expr::kIllegalEA) return;

	// emit the code that tests cases (and in the process assign bytecode
	// labels to any LabelStmts
	SwitchState switchState(selector, defaultLabel);
	SwitchCaseEmitter emitter(b, switchState);
	Apply(GetBody(), emitter);

	// don't need the selector anymore
	b.ReleaseTempEA(selector);

	// we always jump to the default label after all cases
	b.AddJump(defaultLabel);

	// emit body - push partial loop context so 'break' will work
	int bLabel = b.PushFlow(Bytecode::kBreakFlow);
	GetBody()->Emit(b);

	// clean up by setting the break label position
	// and optionally the default label position
	b.SetLabel(bLabel);
	b.PopFlow(Bytecode::kBreakFlow);
	if (!switchState.ContainsCase(CaseStmt::kDefaultValue))
		b.SetLabel(defaultLabel);
}


bool SwitchCaseEmitter::operator()(Stmt *s)
{
	CaseStmt *ls;

	// don't walk into nested switches
	if (dynamic_cast<SwitchStmt*>(s))
		return false;

	// emit labels
	ls = dynamic_cast<CaseStmt*>(s);
	if (ls) ls->EmitSwitchCases(fBytecode, fSwitchState);

	return true;
}


Stmt* SwitchStmt::CloneActual(Mapping *b) const
{
	return new SwitchStmt(fSelector->Clone(b), GetBody()->Clone(b));
}


void SwitchStmt::GetExprs(vector<Expr *> &v) const
{
	v.push_back(fSelector);
}


bool SwitchState::ContainsCase(int v)
{
	size_t i;

	for(i=0; i<fCases.size(); ++i)
		if (fCases[i] == v) return true;

	return false;
}

