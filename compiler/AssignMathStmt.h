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
