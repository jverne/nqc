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
#include <cstring>
#include "AssignStmt.h"
#include "RCX_Cmd.h"
#include "Bytecode.h"
#include "Expr.h"
#include "RCX_Target.h"
#include "Error.h"

AssignStmt::AssignStmt(Expr *lval, Expr * value)
 :	fLval(lval),
	fValue(value)
{
}


AssignStmt::~AssignStmt()
{
	delete fLval;
	delete fValue;
}


void AssignStmt::EmitActual(Bytecode &b)
{
	RCX_Value dst = fLval->EmitAny(b);
	int type = RCX_VALUE_TYPE(dst);

 	if (type == kRCX_VariableType)
	{
                fValue->EmitTo(b, RCX_VALUE_DATA(dst));
	}
	else
	{
		if (b.GetTarget()->SourceWritable(type))
		{
			RCX_Cmd cmd;
			RCX_Value ea = fValue->EmitAny(b);

			cmd.MakeSet(dst, ea);
			b.Add(cmd);

			b.ReleaseTempEA(ea);
		}
		else
			Error(kErr_LValueNeeded).Raise(&fLval->GetLoc());

	}

	b.ReleaseTempEA(dst); // ea may be an array with temp index
}


Stmt* AssignStmt::CloneActual(Mapping *m) const
{
	return new AssignStmt(fLval->Clone(m), fValue->Clone(m));
}


void AssignStmt::GetExprs(vector<Expr *> &v) const
{
	v.push_back(fLval);
	v.push_back(fValue);
}
