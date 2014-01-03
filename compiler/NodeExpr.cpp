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
#include "NodeExpr.h"

NodeExpr::NodeExpr(Expr *e)
	: Expr(e->GetLoc())
{
	fExprs[0] = e;
	fCount = 1;
}


NodeExpr::NodeExpr(Expr *a, Expr *b)
	: Expr(b->GetLoc())
{
	fExprs[0] = a;
	fExprs[1] = b;
	fCount = 2;
}


NodeExpr::NodeExpr(Expr *a, Expr *b, Expr *c)
	: Expr(b->GetLoc())
{
	fExprs[0] = a;
	fExprs[1] = b;
	fExprs[2] = c;
	fCount = 3;
}


NodeExpr::~NodeExpr()
{
	for(int i=0; i<fCount; ++i)
		delete fExprs[i];
}


bool NodeExpr::Contains(int var) const
{
	for(int i=0; i<fCount; ++i)
		if (fExprs[i]->Contains(var)) return true;
	return false;
}


bool NodeExpr::PromiseConstant() const
{
	for(int i=0; i<fCount; ++i)
		if (!fExprs[i]->PromiseConstant()) return false;
	return true;
}


void NodeExpr::GetExprs(vector<Expr*> &v) const
{
	for(int i=0; i<fCount; ++i)
		v.push_back(fExprs[i]);
}
