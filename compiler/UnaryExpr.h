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
#ifndef __UnaryExpr_h
#define __UnaryExpr_h

#include "NodeExpr.h"

class UnaryExpr : public NodeExpr
{
public:
	UnaryExpr(int op, Expr *e) : NodeExpr(e), fOp(op) {}

	virtual bool		Evaluate(int &value) const;
	virtual Expr*		Clone(Mapping *b) const;

	virtual RCX_Value	EmitAny_(Bytecode &b) const;
	virtual bool		EmitTo_(Bytecode &b, int dst) const;
	virtual bool		EmitSide_(Bytecode &b) const;

	static bool	        NeedsConstant(int op);

private:
	int	fOp;
};


#endif
