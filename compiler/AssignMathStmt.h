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
#ifndef __AssignMathStmt_h
#define __AssignMathStmt_h

#ifndef __AssignStmt_h
#include "AssignStmt.h"
#endif

#ifndef __RCX_Constants_h
#include "RCX_Constants.h"
#endif

class AssignMathStmt : public AssignStmt
{
public:
	AssignMathStmt(Expr *lval, RCX_VarCode code, Expr* value);

	void	EmitActual(Bytecode &b);
	Stmt*	CloneActual(Mapping *b) const;

private:
        void	EmitOperation(Bytecode &b, int var);
        void	EmitValueOperation(Bytecode &b, RCX_Value dst);

	RCX_VarCode	fCode;
};


#endif
