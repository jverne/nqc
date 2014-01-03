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
#ifndef __ForStmt_h
#define __ForStmt_h

#ifndef __Stmt_h
#include "Stmt.h"
#endif

#ifndef __Expr_h
#include "Expr.h"
#endif

class ForStmt : public Stmt
{
public:
			ForStmt(Stmt *init, Expr *c, Stmt *iterate, Stmt *body);
			~ForStmt();

	virtual Stmt*	GetChildren()						{ return fChildren.GetHead(); }
	virtual void	GetExprs(vector<Expr*> & v) const	{ if (fCondition) v.push_back(fCondition); }

	virtual void	EmitActual(Bytecode &b);
	virtual Stmt*	CloneActual(Mapping *b) const;


private:
	Stmt*		fInit;
	Expr*		fCondition;
	Stmt*		fIterate;
	Stmt*		fBody;
	PListSS<Stmt> fChildren;
};


#endif
