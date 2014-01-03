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


/* This class is only partially implemented.  Specifically, evaluation always
 * fails even though in theory the value will be constant.  In order to fix this
 * some other virtual such as Expr::GetEA() would need to be added.
 */


#ifndef __EventSrcExpr_h
#define __EventSrcExpr_h

#include "NodeExpr.h"
#include "RCX_Target.h"

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
