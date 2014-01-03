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
#ifndef __RelExpr_h
#define __RelExpr_h

#include "NodeExpr.h"

class RelExpr : public NodeExpr
{
public:
			enum
			{
				kLessOrEqual = 0,
				kGreaterOrEqual,
				kNotEqualTo,
				kEqualTo,
				kGreaterThan,
				kLessThan
			};

			RelExpr(Expr *lhs, int relation, Expr *rhs);

	virtual bool		Evaluate(int &value) const;
	virtual Expr*		Clone(Mapping *b) const;

	// glue to emit values based on EmitBranch_()
	virtual RCX_Value	EmitAny_(Bytecode &b) const { return EmitBoolAny(b); }
	virtual bool		EmitTo_(Bytecode &b, int dst) const { return EmitBoolTo(b, dst); }


	virtual bool		EmitBranch_(Bytecode &b, int label, bool condition) const;

private:
	int			fRelation;
};


#endif
