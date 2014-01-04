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
#include "IndirectExpr.h"
#include "Mapping.h"
#include "RCX_Cmd.h"
#include "Bytecode.h"

Expr* IndirectExpr::Clone(Mapping *m) const
{
	return new IndirectExpr(Get(0)->Clone(m), Get(1)->Clone(m));
}



RCX_Value IndirectExpr::EmitAny_(Bytecode &b) const
{
	// decode src
	int src;
	if (!Get(0)->Evaluate(src))
		return kIllegalEA;
	int directSrc = (src >> 8) & 0xff;
	int indirectSrc = (src) & 0xff;

	// see if direct can be used
	int idx;
	if (directSrc && Get(1)->Evaluate(idx))
        {
		return RCX_VALUE(directSrc, idx);
        }

	// must use indirect

	// see if index is already in a var
	int var = Get(1)->GetLValue();
	if (var != kIllegalVar)
		return RCX_VALUE(indirectSrc, var);

	// emit the index
	int dst = GetTempVar(b);
	if (dst < 0)
		return kIllegalEA;
	Get(1)->EmitTo(b, dst);

	return RCX_VALUE(indirectSrc, dst) + kRCX_ValueUsesTemp;
}
