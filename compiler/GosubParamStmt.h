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
#ifndef __GosubParamStmt_h
#define __GosubParamStmt_h

#ifndef __Stmt_h
#include "Stmt.h"
#endif

#ifndef __parser_h
#include "parser.h"
#endif

#include <vector>

using std::vector;

class Fragment;
class Expr;

class GosubParamStmt : public LeafStmt
{
public:
	GosubParamStmt(Fragment *f, const struct LexLocation &loc) : fFragment(f), fLocation(loc) {}

	bool	Check(CheckState &state);
	void	EmitActual(Bytecode &b);
	Stmt*	CloneActual(Mapping *b) const;
	Fragment*	GetFragment() const	{ return fFragment; }
        void    AddParams(vector<Expr*> &params);

private:
	Fragment*	fFragment;
	struct LexLocation fLocation;
        vector<Expr*>	fParams;

};


#endif
