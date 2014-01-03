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

#include "AtomExpr.h"
#include "Variable.h"
#include "Mapping.h"
#include "Bytecode.h"
#include "Program.h"
#include "RCX_Target.h"

AtomExpr::AtomExpr(RCX_ValueType type, int value, const LexLocation &loc, bool ptr) :
	Expr(loc),
	fType(type),
	fValue(value),
        fPtr(ptr)
{
}


bool AtomExpr::PromiseConstant() const
{
	if (fType == kRCX_ConstantType) return true;
	if ((fType == kRCX_VariableType) &&
		(fValue & kVirtualConstantFlag)) return true;
	return false;
}


bool AtomExpr::Evaluate(int &value) const
{
	if ((fType != kRCX_ConstantType)/* && !fPtr*/) return false;

	value = fValue;
	return true;
}


bool AtomExpr::Contains(int var) const
{
	return (((fType==kRCX_VariableType) || (fType == kRCX_IndirectType)) && fValue==var);
}


bool AtomExpr::PotentialLValue() const
{
	// check for const vars
	if (fType == kRCX_VariableType &&
		fValue & kVirtualReadOnlyFlag) return false;

	// everything else is a potential LValue...make the
	// final determination during code generation
	return true;
}


int AtomExpr::GetLValue() const
{
	if (fType != kRCX_VariableType) return kIllegalVar;
	if (fValue & kVirtualReadOnlyFlag) return kIllegalVar;
//        if (fPtr) return fValue | kPointerFlag;

	return fValue;
}

bool AtomExpr::LValueIsPointer() const
{
  return fPtr;
}


RCX_Value AtomExpr::EmitAny_(Bytecode &) const
{
	return RCX_VALUE(fType, fValue);
}


void AtomExpr::Translate(int from, int to)
{
	if (((fType == kRCX_VariableType) || (fType == kRCX_IndirectType)) &&
		fValue == from)
	{
		fValue = to;
	}
}


Expr* AtomExpr::Clone(Mapping *m) const
{
	if (((fType == kRCX_VariableType) || (fType == kRCX_IndirectType)) && m && (fValue & kVirtualVarBase))
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

	return new AtomExpr(fType, fValue, GetLoc(), fPtr);
}



RCX_Value AtomExpr::GetStaticEA_() const
{
	return RCX_VALUE(fType, fValue);
}
