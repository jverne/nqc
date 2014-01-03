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

#include "RelExpr.h"
#include "Bytecode.h"
/*
#include "parser.h"
#include "RCX_Constants.h"
*/

// these are VERY dependent on RCX_Constants
static int sReversed[] = {
	RelExpr::kGreaterOrEqual, RelExpr::kLessOrEqual, RelExpr::kNotEqualTo,
	RelExpr::kEqualTo, RelExpr::kLessThan, RelExpr::kGreaterThan
};

static int sInverted[] = {
	RelExpr::kGreaterThan, RelExpr::kLessThan, RelExpr::kEqualTo,
	RelExpr::kNotEqualTo, RelExpr::kLessOrEqual, RelExpr::kGreaterOrEqual
};

static RCX_Relation sRCX_Codes[] = {
	kRCX_LessOrEqual, kRCX_GreaterOrEqual, kRCX_NotEqualTo, kRCX_EqualTo
};


RelExpr::RelExpr (Expr *lhs, int relation, Expr *rhs)
	: NodeExpr(lhs, rhs), fRelation(relation)
{
}



Expr* RelExpr::Clone(Mapping *b) const
{
	return new RelExpr(Get(0)->Clone(b), fRelation, Get(1)->Clone(b));
}


bool RelExpr::Evaluate(int &v) const
{
	int v1, v2;
	bool b;

	if (!Get(0)->Evaluate(v1)) return false;
	if (!Get(1)->Evaluate(v2)) return false;

	switch(fRelation)
	{
		case kLessOrEqual:
			b = (v1 <= v2);
			break;
		case kGreaterOrEqual:
			b = (v1 >= v2);
			break;
		case kNotEqualTo:
			b = (v1 != v2);
			break;
		case kEqualTo:
			b = (v1 == v2);
			break;
		case kGreaterThan:
			b = (v1 > v2);
			break;
		case kLessThan:
			b = (v1 < v2);
			break;
		default:
			return false;

	}

	v = b ? 1 : 0;

	return true;
}


bool RelExpr::EmitBranch_(Bytecode &b, int label, bool condition) const
{
	bool ok = true;
	int r = fRelation;
	RCX_Value ea1 = Get(0)->EmitConstrained(b, TEST_MASK);
	RCX_Value ea2 = Get(1)->EmitConstrained(b, TEST_MASK);

	// if value 2 is constant, swap with value 1
	if (RCX_VALUE_TYPE(ea2) == kRCX_ConstantType)
	{
		RCX_Value t = ea1;
		ea1 = ea2;
		ea2 = t;
		r = sReversed[r];
	}

	// invert if emitting false branch
	if (!condition)
		r = sInverted[r];

	switch(r)
	{
		case kLessOrEqual:
		case kGreaterOrEqual:
		case kNotEqualTo:
		case kEqualTo:
			b.AddTest(ea1, sRCX_Codes[r], ea2, label);
			break;
		case kGreaterThan:
		case kLessThan:
			if (RCX_VALUE_TYPE(ea1) == kRCX_ConstantType)
			{
				RCX_Relation rel;
				short adjust;
				short limit;
				short c;

				if (r == kGreaterThan)
				{
					rel = kRCX_GreaterOrEqual;
					adjust = -1;
					limit = -32768;
				}
				else
				{
					rel = kRCX_LessOrEqual;
					adjust = 1;
					limit = 32767;
				}

				c = RCX_VALUE_DATA(ea1);

				// check for impossible range
				if (c == limit)
				{
					ok = false;
					break;
				}

				// test for adjusted range
				b.AddTest(RCX_VALUE(kRCX_ConstantType, c+adjust), rel, ea2, label);
			}
			else
			{
				int around = b.NewLabel();
				b.AddTest(ea1, sRCX_Codes[sInverted[r]], ea2, around);
				b.AddJump(label);
				b.SetLabel(around);
			}
			break;
	}

	b.ReleaseTempEA(ea1);
	b.ReleaseTempEA(ea2);
	return ok;
}
