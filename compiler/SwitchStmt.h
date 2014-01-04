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
#ifndef __SwitchStmt_h
#define __SwitchStmt_h

#ifndef __Stmt_h
#include "Stmt.h"
#endif

#ifndef __Expr_h
#include "Expr.h"
#endif

#include <vector>

using std::vector;

class SwitchStmt : public ChainStmt
{
public:
			SwitchStmt(Expr *e, Stmt *s);
			~SwitchStmt();

	virtual	void	EmitActual(Bytecode &b);

	Stmt*	CloneActual(Mapping *b) const;
	virtual void	GetExprs(vector<Expr*> & v) const;

private:
	Expr*		fSelector;
};


class SwitchState
{
public:
				SwitchState(RCX_Value selector, int defaultLabel) :
					fSelector(selector),
					fDefaultLabel(defaultLabel)
					{}

	bool		ContainsCase(int v);
	void		AddCase(int v)			{ fCases.push_back(v); }

	RCX_Value	GetSelector() const		{ return fSelector; }
	int			GetDefaultLabel() const	{ return fDefaultLabel; }

private:
	RCX_Value	fSelector;
	int			fDefaultLabel;
	vector<int>	fCases;
};

#endif
