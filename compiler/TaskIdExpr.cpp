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
#include "TaskIdExpr.h"
#include "Error.h"
#include "Stmt.h"


Expr* TaskIdExpr::Clone(Mapping *) const
{
	return new TaskIdExpr(GetLoc(), fTaskId);
}


RCX_Value TaskIdExpr::EmitAny_(Bytecode &) const
{
	int v;

	if (!Evaluate(v))
		return kIllegalEA;

	return RCX_VALUE(kRCX_ConstantType, v);
}


bool TaskIdExpr::Evaluate(int &v) const
{
	if (fTaskId < 0)
	{
		// during the parse check phases, ids have not been
		// assigned, so the constant has not yet been
		// determined
		return false;
	}

	v = fTaskId;
	return true;
}


bool TaskIdExpr::Patcher::operator()(Stmt *s)
{
	vector<Expr*> v;
	s->GetExprs(v);

	int n = v.size();
	int i;

	for(i=0; i<n; ++i)
	{
		::Apply(v[i], *this);
	}

	return true;
}


bool TaskIdExpr::Patcher::operator()(Expr *e)
{
	if (TaskIdExpr *te = dynamic_cast<TaskIdExpr*>(e))
	{
		te->fTaskId = fId;
		if (fId < 0)
		{
			// the error must be raised here rather than in Evaluate()
			// since Evaluate() is called prior to task ids being assigned
			Error(kErr_TaskIdUnknown).Raise(&te->GetLoc());
		}
	}

	return true;
}
