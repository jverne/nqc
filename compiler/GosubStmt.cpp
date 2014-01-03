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
