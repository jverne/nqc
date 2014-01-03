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
 * Portions created by David Baum are Copyright (C) 1998 David Baum.
 * All Rights Reserved.
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */

#ifndef __InlineStmt_h
#define __InlineStmt_h

#ifndef __Stmt_h
#include "Stmt.h"
#endif

/*
 * The purpose of the InlineStmt class is to be the root of an
 * expanded inline function.  Its only true funciton is to setup
 * the return label so that return statements within the expanded
 * function will exit to the correct place.
 */

class FunctionDef;

class InlineStmt : public ChainStmt
{
public:
			InlineStmt(Stmt *s, FunctionDef *f) : ChainStmt(s), fFunction(f) { }
	void	EmitActual(Bytecode &b);
	Stmt*	CloneActual(Mapping *b) const;

private:
	FunctionDef*	fFunction;
};


#endif
