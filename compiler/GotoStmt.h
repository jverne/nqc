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
#ifndef __GotoStmt_h
#define __GotoStmt_h

#ifndef __Stmt_h
#include "Stmt.h"
#endif

#ifndef __parser_h
#include "parser.h"
#endif

class Bytecode;

class GotoStmt : public LeafStmt
{
public:
			GotoStmt(const Symbol *name, const struct LexLocation &loc);

	bool	Check(CheckState &state);
	void	EmitActual(Bytecode &b);
	Stmt*	CloneActual(Mapping *b) const;

	void	SetLabel(int l)	{ fLabel = l; }

	static void ResolveGotos(Stmt *s, Bytecode &b);

	friend class GotoBinder;

private:
	const Symbol*		fName;
	struct LexLocation	fLocation;
	int					fLabel;

};


#endif
