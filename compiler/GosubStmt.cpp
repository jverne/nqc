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
#include "GosubStmt.h"
#include "Bytecode.h"
#include "Fragment.h"
#include "Error.h"
#include "RCX_Cmd.h"

void GosubStmt::EmitActual(Bytecode &b)
{
	if (!b.GetVarAllocator().CheckLocalMask(fFragment->GetLocalMask()))
	{
		Error(kErr_NoMoreVars).Raise(&fLocation);
		return;
	}

	RCX_Cmd cmd;

	cmd.Set(kRCX_GoSubOp, fFragment->GetNumber());
	b.Add(cmd);

}


Stmt* GosubStmt::CloneActual(Mapping *) const
{
	return new GosubStmt(fFragment, fLocation);
}
