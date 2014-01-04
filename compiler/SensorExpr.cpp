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
#include "SensorExpr.h"
#include "Bytecode.h"
#include "Error.h"


Expr* SensorExpr::Clone(Mapping *b) const
{
	return new SensorExpr(Get(0)->Clone(b));
}


RCX_Value SensorExpr::EmitAny_(Bytecode &) const
{
	int v;

	if (!Evaluate(v))
		return kIllegalEA;

	return RCX_VALUE(kRCX_ConstantType, v);
}


bool SensorExpr::Evaluate(int &v) const
{
	RCX_Value ea;

	ea = Get(0)->GetStaticEA();
	v = -1;

	switch(RCX_VALUE_TYPE(ea))
	{
		case kRCX_ConstantType:
		case kRCX_InputValueType:
			v = RCX_VALUE_DATA(ea) & 0xff;
			return true;
		default:
			Error(kErr_ParamType, "sensor").Raise(&GetLoc());
			return false;
	}
}
