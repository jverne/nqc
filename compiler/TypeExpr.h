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
 * Portions created by David Baum are Copyright (C) 1999 David Baum.
 * All Rights Reserved.
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */


/* This class is only partially implemented.  Constant expressions always
 * evalutae correctly to type 2.  All other expressions evaluate to type 0
 * even if they are from some other non-constant source (sensor, random, etc)
 *
 * Fixing this would require either adding RTTI code, or preferably adding
 * another virtual method to the Expr hierarchy that returns the typecode of
 * the expression.
 */


#ifndef __TypeExpr_h
#define __TypeExpr_h

#include "NodeExpr.h"


class TypeExpr : public NodeExpr
{
public:
		TypeExpr(Expr *e) : NodeExpr(e) {}

	virtual Expr*		Clone(Mapping *b) const;

	virtual RCX_Value	EmitAny_(Bytecode &b) const;

	virtual bool		PromiseConstant() const	{ return true; }
	virtual bool		Evaluate(int & value) const;
};


#endif
