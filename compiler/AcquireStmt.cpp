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

#include "AcquireStmt.h"
#include "Bytecode.h"
#include "Expr.h"
#include "RCX_Cmd.h"
#include "RCX_Target.h"
#include "Error.h"

AcquireStmt::AcquireStmt(UByte resources, Stmt* body, Stmt *handler, const LexLocation &loc) :
	BinaryStmt(body, handler), fResources(resources), fLocation(loc)
{
}


void AcquireStmt::EmitActual(Bytecode &b)
{
	if (!b.GetTarget()->fResources)
	{
		Error(kErr_NoTargetResources, b.GetTarget()->fName).Raise(&fLocation);
		return;
	}

	if (!b.BeginHandler(Bytecode::kAcquireHandler))
	{
		Error(kErr_NoNestedResources).Raise(&fLocation);
		return;
	}

	RCX_Cmd cmd;
	int handlerLabel = b.NewLabel();
	int endLabel = b.NewLabel();

	// start event monitoring
	cmd.Set(kRCX_EnterAccessCtrlOp, fResources, 0, 0);
	b.Add(cmd);
	b.AddFixup(Bytecode::kSignBitLongFixup, handlerLabel);

	GetPrimary()->Emit(b);

	// end event monitoring
	cmd.Set(kRCX_ExitAccessCtrlOp);
	b.Add(cmd);

	if (GetSecondary())
		b.AddJump(endLabel);

	// here's the handler
	b.SetLabel(handlerLabel);

	if (GetSecondary())
		GetSecondary()->Emit(b);

	b.SetLabel(endLabel);

	b.EndHandler(Bytecode::kAcquireHandler);
}


Stmt* AcquireStmt::CloneActual(Mapping *b) const
{
	Stmt *s = GetSecondary() ? GetSecondary()->Clone(b) : 0;
	return new AcquireStmt(fResources, GetPrimary()->Clone(b), s, fLocation);
}
