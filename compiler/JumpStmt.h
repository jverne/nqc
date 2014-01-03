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
#ifndef __JumpStmt_h
#define __JumpStmt_h

#ifndef __Stmt_h
#include "Stmt.h"
#endif

#ifndef __parser_h
#include "parser.h"
#endif


class JumpStmt : public LeafStmt
{
public:
			JumpStmt(int type, const LexLocation &loc);

	void	EmitActual(Bytecode &b);
	Stmt*	CloneActual(Mapping *b) const;

private:
	int		fType;
	LexLocation	fLocation;
};

/*
enum
{
	kJump_Break = 0,
	kJump_Continue,

	kJump_Count		// must be the last one
};
*/

#endif
