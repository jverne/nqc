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
#include "NegateExpr.h"



Expr* NegateExpr::Clone(Mapping *b) const
{
	return new NegateExpr(Get(0)->Clone(b));
}


bool NegateExpr::EmitBranch_(Bytecode &b, int label, bool condition) const
{
	return Get(0)->EmitBranch(b, label, !condition);
}


bool NegateExpr::Evaluate(int &value) const
{
	if (!Get(0)->Evaluate(value)) return false;

	value = value ? 0 : 1;

	return true;
}
