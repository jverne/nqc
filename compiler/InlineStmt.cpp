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


Stmt* InlineStmt::CloneActual(Mapping *b) const
{
	// this never really gets called because cloning happens before
	// InlineStmts are substituted for CallStmts.

	return new InlineStmt(GetBody()->Clone(b), fFunction);
}
