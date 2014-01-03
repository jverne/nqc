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
 * Portions created by David Baum are Copyright (C) 1998 David Baum.
 * All Rights Reserved.
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */

#ifndef __ExprStmt_h
#define __ExprStmt_h

#ifndef __Stmt_h
#include "Stmt.h"
#endif

class Expr;

class ExprStmt : public LeafStmt
{
public:
				ExprStmt(Expr* value);
				~ExprStmt();

	void		EmitActual(Bytecode &b);
	Stmt*		CloneActual(Mapping *b) const;
	virtual void	GetExprs(vector<Expr*> & v) const;

private:
	Expr*		fValue;
};


#endif
