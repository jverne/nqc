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

#ifndef __CatchStmt_h
#define __CatchStmt_h

#ifndef __Stmt_h
#include "Stmt.h"
#endif

#ifndef __parser_h
#include "parser.h"
#endif

class SwitchState;

class CatchStmt : public ChainStmt
{
public:
			CatchStmt(int v, Stmt *s, const LexLocation &loc);
			~CatchStmt();

	virtual void	EmitActual(Bytecode &b);
	virtual Stmt*	CloneActual(Mapping *b) const;

	void			SetLabel(int l)	{ fLabel = l; }

private:
	int			fValue;
	LexLocation fLocation;
	int			fLabel;
};


#endif
