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


/*
 * LabelStmts are used to bind a label to another statement.  The
 * LabelStmt isn't really a statement itself - just a wrapper for
 * a single child statement.
 *
 */

#ifndef __LabelStmt_h
#define __LabelStmt_h

#ifndef __Stmt_h
#include "Stmt.h"
#endif

#ifndef __parser_h
#include "parser.h"
#endif


class LabelStmt : public ChainStmt
{
public:
			LabelStmt(const Symbol *name, const LexLocation &loc, Stmt *s = 0);
			~LabelStmt();

	virtual void	EmitActual(Bytecode &b);
	virtual Stmt*	CloneActual(Mapping *b) const;

	const Symbol*	GetName() const	{ return fName; }
	int				GetLabel() const { return fLabel; }

	friend class LabelFinder;

private:
	const Symbol*	fName;
	LexLocation		fLocation;
	int				fLabel;
};


#endif
