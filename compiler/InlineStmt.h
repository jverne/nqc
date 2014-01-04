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
