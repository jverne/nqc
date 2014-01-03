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
#include "TaskStmt.h"
#include "Bytecode.h"
#include "Symbol.h"
#include "Fragment.h"
#include "Program.h"
#include "Error.h"
#include "RCX_Cmd.h"

TaskStmt::TaskStmt(UByte opcode, Symbol *name, const LexLocation &loc)
{
	fOpcode = opcode;
	fName = name;
	fLocation = loc;
}


void TaskStmt::EmitActual(Bytecode &b)
{
	Fragment *f = gProgram->GetTask(fName);
	RCX_Cmd cmd;

	if (!f)
	{
		Error(kErr_UndefinedTask, fName->GetKey()).Raise(&fLocation);
		return;
	}

	cmd.Set(fOpcode, (UByte)f->GetNumber());
	b.Add(cmd);
}


Stmt* TaskStmt::CloneActual(Mapping *) const
{
	return new TaskStmt(fOpcode, fName, fLocation);
}
