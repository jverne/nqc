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
#include "AssignMathStmt.h"
#include "RCX_Cmd.h"
#include "Bytecode.h"
#include "Expr.h"
#include "Error.h"


AssignMathStmt::AssignMathStmt(Expr *lval, RCX_VarCode code, Expr * value)
 :  AssignStmt(lval, value),
    fCode(code)
{
}

void AssignMathStmt::EmitActual(Bytecode &b)
{
    RCX_Value dst = fLval->EmitAny(b);

    if (RCX_VALUE_TYPE(dst) == kRCX_VariableType) {
        EmitOperation(b, RCX_VALUE_DATA(dst));
    }
    else if (b.GetTarget()->fHasExtendedMathOps && (fCode != kRCX_IllegalVar)) {
        // this firmware supports math assignment to generic sources
        EmitValueOperation(b, dst);
    } else
        Error(kErr_NotSupported, "math assignment to non-vars").Raise(&fLval->GetLoc());

    b.ReleaseTempEA(dst);
}



void AssignMathStmt::EmitOperation(Bytecode &b, int var)
{
    RCX_Value ea = fValue->EmitMath(b);
    if (ea != Expr::kIllegalEA) {
        RCX_Cmd cmd;
        cmd.MakeVar(fCode, (UByte)var, ea);
        b.Add(cmd);
        b.ReleaseTempEA(ea);
    }
}

void AssignMathStmt::EmitValueOperation(Bytecode &b, RCX_Value dst)
{
    RCX_Value ea = fValue->EmitMath(b);
    if (ea != Expr::kIllegalEA){
        RCX_Cmd cmd;
        cmd.MakeSetMath(dst, ea, fCode);
        b.Add(cmd);
        b.ReleaseTempEA(ea);
    }
}


Stmt* AssignMathStmt::CloneActual(Mapping *m) const
{
    return new AssignMathStmt(fLval->Clone(m), fCode, fValue->Clone(m));
}
