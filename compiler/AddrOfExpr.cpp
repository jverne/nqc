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
 * The Initial Developer of this code is John Hansen.
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
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
        if (m && (fValue & kVirtualVarBase))
        {
                const Expr *e = m->Get(fValue);
                if (e)
                {
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
