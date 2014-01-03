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
