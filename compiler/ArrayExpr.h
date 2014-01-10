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

    virtual Expr*       Clone(Mapping *b) const;
    virtual bool        PromiseConstant() const  { return false; }
    virtual bool        PotentialLValue() const { return true; }

    virtual RCX_Value   EmitAny_(Bytecode &b) const;

    virtual void        Translate(int from, int to);

private:
    int     fVar;
};


#endif
