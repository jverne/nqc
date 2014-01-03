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
 * Portions created by John Hansen are Copyright (C) 2004 John Hansen.
 * All Rights Reserved.
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */

#include "DerefExpr.h"
#include "Variable.h"
#include "Mapping.h"
#include "Bytecode.h"
#include "Program.h"
#include "RCX_Target.h"
#include "Error.h"

DerefExpr::DerefExpr(int value, const LexLocation &loc) :
        Expr(loc),
        fValue(value)
{
}


bool DerefExpr::PromiseConstant() const
{
        if (fValue & kVirtualConstantFlag) return true;
        return false;
}


bool DerefExpr::Evaluate(int &) const
{
        return false;
}


bool DerefExpr::Contains(int var) const
{
        return (fValue==var);
}


bool DerefExpr::PotentialLValue() const
{
        return true;
}


int DerefExpr::GetLValue() const
{
        const RCX_Target *t = gProgram->GetTarget();
        if (t->fHasExtendedMathOps)
        {
            return fValue;
        }
        else
            return kIllegalVar;
}


RCX_Value DerefExpr::EmitAny_(Bytecode &) const
{
        return RCX_VALUE(kRCX_IndirectType, fValue);
}


void DerefExpr::Translate(int from, int to)
{
        if (fValue == from)
                fValue = to;
}


Expr* DerefExpr::Clone(Mapping *m) const
{
        if (m && (fValue & kVirtualVarBase))
        {
                const Expr *e = m->Get(fValue);
                if (e)
                {
                  // a deref expression always needs to return a deref expr clone
                  // regardless of the type of expression in the Mapping
                  // the expression in the map is used only to pass us the variable number
                  int val;
                  if (!e->Evaluate(val))
                   {
                       val = e->GetLValue();
                       if ((val == kIllegalVar) /* || !(val & kPointerFlag) */)
                       {
                           Error(kErr_ParamType, "pointer").Raise(&e->GetLoc());
                           return e->Clone(0);
                       }
                   }
//                   return new DerefExpr(((val & kPointerFlag) ? val - kPointerFlag : val), GetLoc());
                   return new DerefExpr(val, GetLoc());
                }
        }

        return new DerefExpr(fValue, GetLoc());
}



RCX_Value DerefExpr::GetStaticEA_() const
{
        return RCX_VALUE(kRCX_IndirectType, fValue);
}
