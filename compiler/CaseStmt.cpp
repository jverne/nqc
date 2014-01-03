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
#include "CaseStmt.h"
#include "Bytecode.h"
#include "Error.h"
#include "SwitchStmt.h"

CaseStmt::CaseStmt(int v, const LexLocation &loc, Stmt *s) :
	ChainStmt(s),
	fValue(v),
	fLocation(loc),
	fLabel(kIllegalLabel)
{
}


CaseStmt::~CaseStmt()
{
}


void CaseStmt::EmitActual(Bytecode &b)
{
	if (fLabel == kIllegalLabel)
	{
		Error(kErr_NoSwitch).Raise(&fLocation);
		return;
	}

	b.SetLabel(fLabel);

	GetBody()->Emit(b);
}


void CaseStmt::EmitSwitchCases(Bytecode &b, SwitchState &state)
{
	// check for duplicate case
	if (state.ContainsCase(fValue))
	{
		Error(kErr_DuplicateCase).Raise(&fLocation);
	}

	// add the case
	state.AddCase(fValue);

	if (fValue == kDefaultValue)
	{
		// don't emit a test - just remember this is the default case
		fLabel = state.GetDefaultLabel();
	}
	else
	{
		// create label for the case and emit the test
		fLabel = b.NewLabel();
		b.AddTest(RCX_VALUE(kRCX_ConstantType, fValue), kRCX_EqualTo, state.GetSelector(), fLabel);
	}
}


Stmt* CaseStmt::CloneActual(Mapping *m) const
{
	return new CaseStmt(fValue, fLocation, GetBody()->Clone(m));
}
