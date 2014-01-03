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

#include "ValueExpr.h"



Expr* ValueExpr::Clone(Mapping *b) const
{
	return new ValueExpr(Get(0)->Clone(b));
}


RCX_Value ValueExpr::EmitAny_(Bytecode &) const
{
	int v;

	if (!Get(0)->Evaluate(v)) return kIllegalEA;

	return (RCX_Value)v;
}


RCX_Value ValueExpr::GetStaticEA_() const
{
	int v;

	if (!Get(0)->Evaluate(v)) return kIllegalEA;

	return v;
}
