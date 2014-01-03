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
