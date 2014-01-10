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

#include "InlineStmt.h"
#include "Bytecode.h"
#include "FunctionDef.h"

void InlineStmt::EmitActual(Bytecode &b)
{
    b.AddSourceTag(
        fFunction->GetListingEnabled() ? RCX_SourceTag::kBegin : RCX_SourceTag::kBeginNoList,
        fFunction->GetStartLoc());

    int rLabel = b.PushFlow(Bytecode::kReturnFlow);
    b.PushFlow(Bytecode::kContinueFlow, false);
    b.PushFlow(Bytecode::kBreakFlow, false);

    GetBody()->Emit(b);

    b.SetLabel(rLabel);

    b.PopFlow(Bytecode::kContinueFlow);
    b.PopFlow(Bytecode::kBreakFlow);
    b.PopFlow(Bytecode::kReturnFlow);

    b.AddSourceTag(RCX_SourceTag::kEnd, fFunction->GetEndLoc());
}


/// This never really gets called because cloning happens before
/// InlineStmts are substituted for CallStmts.
Stmt* InlineStmt::CloneActual(Mapping *b) const
{
    return new InlineStmt(GetBody()->Clone(b), fFunction);
}
