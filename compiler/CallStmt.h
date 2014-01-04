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
#ifndef __CallStmt_h
#define __CallStmt_h

#ifndef __Stmt_h
#include "Stmt.h"
#endif

#ifndef __parser_h
#include "parser.h"
#endif

#include <vector>

using std::vector;

class Symbol;
class Expr;

class CallStmt : public ChainStmt
{
public:
			CallStmt();
			~CallStmt();

	void		EmitActual(Bytecode &b);
	Stmt*		CloneActual(Mapping *b) const;

	virtual void	GetExprs(vector<Expr*> & v) const;

	void	SetName(const Symbol *name)	{ fName = name; }
	void	SetLocation(const struct LexLocation &loc)	{ fLocation = loc; }
	void	AddParam(Expr *c)			{ fParams.push_back(c); }

	void	Expand(Fragment *f);

	class Expander
	{
	public:
				Expander(Fragment *f) : fFragment(f) {}
		bool	operator()(Stmt *s);

	private:
		Fragment*	fFragment;
	};

private:

	void	ExpandFunction(FunctionDef *func, Fragment *fragment);

	const Symbol*	fName;
	struct LexLocation fLocation;
	vector<Expr*>	fParams;
};




#endif
