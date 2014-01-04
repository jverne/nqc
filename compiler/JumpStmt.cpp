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
#include "JumpStmt.h"
#include "Bytecode.h"
#include "Error.h"


JumpStmt::JumpStmt(int type, const LexLocation &loc)
{
	fType = type;
	fLocation = loc;
}


void JumpStmt::EmitActual(Bytecode &b)
{
	if (!b.AddFlowJump(static_cast<Bytecode::FlowCode>(fType)))
	{
		Error(
			(fType==Bytecode::kContinueFlow) ? kErr_NoContinueContext : kErr_NoBreakContext
			).Raise(&fLocation);
	}
}


Stmt* JumpStmt::CloneActual(Mapping *) const
{
	return new JumpStmt(fType, fLocation);
}
