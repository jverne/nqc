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


#ifndef __AtomExpr_h
#define __AtomExpr_h

#ifndef __Expr_h
#include "Expr.h"
#endif

#ifndef __VarTranslator_h
#include "VarTranslator.h"
#endif

class AtomExpr : public Expr, public Translatable
{
public:
		AtomExpr(RCX_ValueType type, int value, const LexLocation &loc, bool ptr = false);


	virtual bool		PromiseConstant() const;

	virtual bool		Evaluate(int &value) const;
	virtual Expr*		Clone(Mapping *b) const;

	virtual bool		PotentialLValue() const;
	virtual int			GetLValue() const;
        virtual bool            LValueIsPointer() const;

	virtual bool		Contains(int var) const;
	virtual RCX_Value	EmitAny_(Bytecode &b) const;
	virtual RCX_Value	GetStaticEA_() const;

	void				Translate(int from, int to);
private:
	RCX_ValueType	fType;
	int				fValue;
        bool fPtr;
};


#endif
