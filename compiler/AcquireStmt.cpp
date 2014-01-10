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
    if (!b.GetTarget()->fResources) {
        Error(kErr_NoTargetResources, b.GetTarget()->fName).Raise(&fLocation);
        return;
    }

    if (!b.BeginHandler(Bytecode::kAcquireHandler)) {
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
