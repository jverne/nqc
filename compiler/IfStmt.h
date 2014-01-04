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
#ifndef __IfStmt_h
#define __IfStmt_h

#ifndef __Stmt_h
#include "Stmt.h"
#endif

#ifndef __Expr_h
#include "Expr.h"
#endif

class IfStmt : public BinaryStmt
{
public:
			IfStmt(Expr *c, Stmt *s1, Stmt *s2=nil);
			~IfStmt();

	void	EmitActual(Bytecode &b);
	Stmt*	CloneActual(Mapping *b) const;

	virtual void	GetExprs(vector<Expr*> & v) const	{ v.push_back(fCondition); }

private:
	void	EmitIf(Bytecode &b);
	void	EmitIfElse(Bytecode &b);

	Expr*	fCondition;
};


#endif
