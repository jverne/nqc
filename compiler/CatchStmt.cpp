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
