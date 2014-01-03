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

#ifndef __NodeExpr_h
#define __NodeExpr_h

#ifndef __Expr_h
#include "Expr.h"
#endif

class NodeExpr : public Expr
{
public:
		NodeExpr(Expr *e);
		NodeExpr(Expr *a, Expr *b);
		NodeExpr(Expr *a, Expr *b, Expr *c);
		~NodeExpr();

	virtual bool		Contains(int var) const;
	virtual bool		PromiseConstant() const;
	virtual void		GetExprs(vector<Expr*> & /* v */) const;

protected:
	Expr*		Get(int i)			{ return fExprs[i]; }
	const Expr*	Get(int i) const	{ return fExprs[i]; }

private:
	static const int MAX_EXPRS = 3;
	Expr*	fExprs[MAX_EXPRS];
	int		fCount;
};


#endif
