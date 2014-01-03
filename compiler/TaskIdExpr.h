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
