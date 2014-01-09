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

/**
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
