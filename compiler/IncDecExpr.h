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


#ifndef __IncDecExpr_h
#define __IncDecExpr_h

#ifndef __Expr_h
#include "Expr.h"
#endif

#ifndef __VarTranslator_h
#include "VarTranslator.h"
#endif

class IncDecExpr : public Expr, public Translatable
{
public:
            IncDecExpr(int var, bool inc, bool pre, const LexLocation &loc);
            ~IncDecExpr();

    virtual Expr*       Clone(Mapping *b) const;
    virtual bool        Contains(int var) const;
    virtual bool        PromiseConstant() const { return false; }
    virtual RCX_Value   EmitAny_(Bytecode &b) const;
    virtual bool        EmitTo_(Bytecode &b, int dst) const;
    virtual bool        EmitSide_(Bytecode &b) const;

    void                Translate(int from, int to);
private:
    int         fVar;
    bool        fInc;
    bool        fPre;
};



#endif
