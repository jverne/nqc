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
