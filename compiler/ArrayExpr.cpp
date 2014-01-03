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
 * Portions created by David Baum are Copyright (C) 2000 David Baum.
 * All Rights Reserved.
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */

#include "ArrayExpr.h"
#include "Mapping.h"
#include "RCX_Cmd.h"
#include "Bytecode.h"
#include "Program.h"

Expr* ArrayExpr::Clone(Mapping *m) const
{
	int newVar = m ? m->GetMappedVar(fVar) : fVar;
	return new ArrayExpr(newVar, Get(0)->Clone(m));
}


RCX_Value ArrayExpr::EmitAny_(Bytecode &b) const
{
        int value;

        if (Get(0)->Evaluate(value))
        {
                return RCX_VALUE(kRCX_VariableType,fVar + value);
        }

        // emit the index
        int dst = GetTempVar(b);
        if (dst < 0)
                return kIllegalEA;
        Get(0)->EmitTo(b, dst);

        // adjust for base of array
        if (fVar)
        {
                RCX_Cmd cmd;
                cmd.MakeVar(kRCX_AddVar,dst, RCX_VALUE(kRCX_ConstantType,fVar));
                b.Add(cmd);
        }

        return RCX_VALUE(kRCX_IndirectType,dst) + kRCX_ValueUsesTemp;
}


void ArrayExpr::Translate(int from, int to)
{
	if (fVar == from)
		fVar = to;
}
