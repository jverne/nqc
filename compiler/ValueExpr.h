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

#ifndef __ValueExpr_h
#define __ValueExpr_h

#include "NodeExpr.h"

class ValueExpr : public NodeExpr
{
public:
		ValueExpr(Expr *e) : NodeExpr(e) {}

	virtual Expr*		Clone(Mapping *b) const;
	virtual bool		PromiseConstant() const	 { return false; }

	virtual bool		PotentialLValue() const	{ return true; }

	virtual RCX_Value	EmitAny_(Bytecode &b) const;
	virtual RCX_Value	GetStaticEA_() const;
};


#endif
