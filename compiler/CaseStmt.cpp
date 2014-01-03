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
