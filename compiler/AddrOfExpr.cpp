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
#include "AddrOfExpr.h"
#include "Variable.h"
#include "Mapping.h"
#include "Bytecode.h"
#include "RCX_Target.h"

AddrOfExpr::AddrOfExpr(int value, int offset, const LexLocation &loc) :
        Expr(loc),
        fValue(value),
        foffset(offset)
{
}


bool AddrOfExpr::PromiseConstant() const
{
//        if (fValue & kVirtualConstantFlag) return true;
//        return false;
  return true;
}


bool AddrOfExpr::Evaluate(int &value) const
{
    if (fValue & kVirtualConstantFlag) return false;
        value = fValue+foffset;
    return true;
}


bool AddrOfExpr::Contains(int var) const
{
    return (fValue==var);
}


RCX_Value AddrOfExpr::EmitAny_(Bytecode &) const
{
    return RCX_VALUE(kRCX_ConstantType, fValue);
}


void AddrOfExpr::Translate(int from, int to)
{
    if (fValue == from)
        fValue = to;
}


Expr* AddrOfExpr::Clone(Mapping *m) const
{
    if (m && (fValue & kVirtualVarBase)) {
        const Expr *e = m->Get(fValue);
        if (e) {
            Expr *cloned = e->Clone(0);

            // I can't decide if substituted vars from inline functions
            // should bind their location to the expr within the function
            // or the actual parameter.  For actual parameter, do nothing,
            // for expr within function, set the location of the cloned
            // expression.
            cloned->SetLoc(GetLoc());
            return cloned;
        }
    }

    return new AddrOfExpr(fValue, foffset, GetLoc());
}


RCX_Value AddrOfExpr::GetStaticEA_() const
{
    return RCX_VALUE(kRCX_ConstantType, fValue+foffset);
}

bool AddrOfExpr::LValueIsPointer() const
{
  return true;
}
