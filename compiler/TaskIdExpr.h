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

#ifndef __TaskIdExpr_h
#define __TaskIdExpr_h

#include "Expr.h"
#include "Fragment.h"

class TaskIdExpr : public Expr
{
public:
						TaskIdExpr(const LexLocation &loc, int id=Fragment::kNoTaskID) : Expr(loc), fTaskId(id) {}

	virtual Expr*		Clone(Mapping *b) const;

	virtual RCX_Value	EmitAny_(Bytecode &b) const;

	virtual bool		Evaluate(int &) const;

	class Patcher
	{
	public:
				Patcher(int id) { fId = id; }

		bool	operator()(Stmt *s);
		bool 	operator()(Expr *e);

	private:
		int		fId;
	};

	friend class Patcher;

private:
	int		fTaskId;
};


#endif
