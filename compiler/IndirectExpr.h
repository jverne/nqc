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

#ifndef __IndirectExpr_h
#define __IndirectExpr_h

#include "NodeExpr.h"


class IndirectExpr : public NodeExpr
{
public:
						IndirectExpr(Expr *src, Expr *idx) : NodeExpr(src, idx) {}

	virtual Expr*		Clone(Mapping *b) const;
	virtual bool		PromiseConstant() const	 { return false; }
	virtual bool		PotentialLValue() const	{ return true; }

	virtual RCX_Value	EmitAny_(Bytecode &b) const;

private:
};


#endif
