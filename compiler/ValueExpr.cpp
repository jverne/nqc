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
