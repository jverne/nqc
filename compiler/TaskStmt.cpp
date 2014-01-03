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
