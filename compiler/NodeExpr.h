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
