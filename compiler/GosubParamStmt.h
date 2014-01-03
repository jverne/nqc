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
 * The Initial Developer of this code is John Hansen.
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
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
