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


#ifndef __ArrayExpr_h
#define __ArrayExpr_h

#include "NodeExpr.h"

#ifndef __VarTranslator_h
#include "VarTranslator.h"
#endif


class ArrayExpr : public NodeExpr, public Translatable
{
public:
		ArrayExpr(int var, Expr *e) : NodeExpr(e), fVar(var) {}

	virtual Expr*		Clone(Mapping *b) const;
	virtual bool		PromiseConstant() const	 { return false; }
	virtual bool		PotentialLValue() const	{ return true; }

	virtual RCX_Value	EmitAny_(Bytecode &b) const;

	virtual void		Translate(int from, int to);

private:
	int		fVar;
};


#endif
