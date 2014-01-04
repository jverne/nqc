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
#include "CatchStmt.h"
#include "Bytecode.h"
#include "Error.h"
#include "RCX_Cmd.h"
#include "RCX_Target.h"
#include "Variable.h"

CatchStmt::CatchStmt(int v, Stmt *s, const LexLocation &loc) :
	ChainStmt(s),
	fValue(v),
	fLocation(loc)
{
}


CatchStmt::~CatchStmt()
{
}


void CatchStmt::EmitActual(Bytecode &b)
{
	if ((b.GetTarget()->fType != kRCX_RCX2Target) && (b.GetTarget()->fType != kRCX_SwanTarget))
	{
		Error(kErr_NoTargetPartialCatch, b.GetTarget()->fName).Raise(&fLocation);
		return;
	}

	int var = b.GetTempVar(true);

	if (var == kIllegalVar)
	{
		Error(kErr_NoMoreTemps).Raise(&fLocation);
		return;
	}

	RCX_Value ea = RCX_VALUE(kRCX_VariableType, var);

	b.ReleaseTempEA(ea);

	// move active events into temp
	b.AddMove(var, RCX_VALUE(0x17, 0x0a));

	// and with mask
	RCX_Cmd cmd;
	cmd.MakeVar(kRCX_AndVar, var, RCX_VALUE(kRCX_ConstantType, fValue));
	b.Add(cmd);

	// jump if zero
	b.AddTest(RCX_VALUE(kRCX_ConstantType, 0), kRCX_EqualTo, RCX_VALUE(kRCX_VariableType, var), fLabel);

	GetBody()->Emit(b);
}


Stmt* CatchStmt::CloneActual(Mapping *m) const
{
	return new CatchStmt(fValue, GetBody()->Clone(m), fLocation);
}
