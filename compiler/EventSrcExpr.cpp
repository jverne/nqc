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

#include "EventSrcExpr.h"
#include "Bytecode.h"
#include "Error.h"

static int ConvertRcx2Source(int type, int data);
static int ConvertSpyboticsSource(int type, int data);


Expr* EventSrcExpr::Clone(Mapping *b) const
{
	return new EventSrcExpr(Get(0)->Clone(b), fTargetType);
}


RCX_Value EventSrcExpr::EmitAny_(Bytecode &) const
{
	// this function only gets called after Evaluate() has failed,
	// so we can just return an error without trying to Evaluate
	return kIllegalEA;
}


bool EventSrcExpr::Evaluate(int &v) const
{
	RCX_Value ea;

	ea = Get(0)->GetStaticEA();
	v = -1;

	if (ea != kIllegalEA)
	{
		int data = RCX_VALUE_DATA(ea);
		int type = RCX_VALUE_TYPE(ea);

		switch(fTargetType) {
			case kRCX_RCX2Target:
                        case kRCX_SwanTarget:
				v = ConvertRcx2Source(type, data);
				break;
			case kRCX_SpyboticsTarget:
				v = ConvertSpyboticsSource(type, data);
				break;
			default:
				v = -1;
				break;
		}
	}

	if (v==-1)
	{
		Error(kErr_BadEventSource).Raise(&GetLoc());
	}

	return true;
}


int ConvertRcx2Source(int type, int data)
{
	switch(type)
	{
		case kRCX_InputValueType:
			if (data >= 0 && data <=2)
				return data;
			break;
		case kRCX_TimerType:
			if (data >=0 && data <= 3)
				return 3 + data;
			break;
		case kRCX_MessageType:
			if (data == 0)
				return 7;
			break;
		case kRCX_CounterType:
			if (data >=0 && data <= 2)
				return 8 + data;
			break;
		default:	// don't do anything...just prevent warnings
			break;
	}

	return -1;
}



int ConvertSpyboticsSource(int type, int data)
{
	switch(type)
	{
		case kRCX_InputValueType:
			if (data >= 0 && data <=1)
				return data;
			break;
		case kRCX_TimerType:
			if (data >=0 && data <= 3)
				return 2 + data;
			break;
		case kRCX_CounterType:
			if (data >=0 && data <= 2)
				return 6 + data;
			break;
		case kRCX_MessageType:
			if (data == 0)
				return 9;
			break;
	}

	return -1;
}
