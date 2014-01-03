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
 * Portions created by David Baum are Copyright (C) 2000 David Baum.
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
