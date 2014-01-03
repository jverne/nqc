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
