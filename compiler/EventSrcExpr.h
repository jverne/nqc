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

#ifndef __EventSrcExpr_h
#define __EventSrcExpr_h

#include "NodeExpr.h"
#include "RCX_Target.h"

/**
 * This class is only partially implemented. Specifically, evaluation always
 * fails even though in theory the value will be constant. In order to fix this
 * some other virtual such as Expr::GetEA() would need to be added.
 */
class EventSrcExpr : public NodeExpr
{
public:
	EventSrcExpr(Expr *e, RCX_TargetType targetType) : NodeExpr(e), fTargetType(targetType) {}

	virtual Expr*		Clone(Mapping *b) const;

	virtual RCX_Value	EmitAny_(Bytecode &b) const;

	virtual bool		PromiseConstant() const	{ return true; }
	virtual bool		Evaluate(int &) const;

private:
	RCX_TargetType		fTargetType;
};


#endif
