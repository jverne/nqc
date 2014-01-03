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
#include "TypeExpr.h"



Expr* TypeExpr::Clone(Mapping *b) const
{
	return new TypeExpr(Get(0)->Clone(b));
}


RCX_Value TypeExpr::EmitAny_(Bytecode &) const
{
	return kIllegalEA;
}


bool TypeExpr::Evaluate(int &value) const
{
	int v;

	if (Get(0)->Evaluate(v))
		value = 2;
	else
		value = 0;

	return true;
}
