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
 * CaseStmts are used to bind a case label to another statement.  The
 * CaseStmt isn't really a statement itself - just a wrapper for
 * a single child statement.
 *
 * Upon construction CaseStmts are assigned their value and location.
 * During code generation of a switch statement, actual labels are
 * assigned to the CaseStmts within the switch.  Code generation for
 * CaseStmt itself only consists of setting the designated label to the
 * current bytecode locaiton.
 *
 */

#ifndef __CaseStmt_h
#define __CaseStmt_h

#ifndef __Stmt_h
#include "Stmt.h"
#endif

#ifndef __parser_h
#include "parser.h"
#endif

class SwitchState;

class CaseStmt : public ChainStmt
{
public:
	enum
	{
		kDefaultValue = 0x10000	// value for 'default:'
	};

			CaseStmt(int v, const LexLocation &loc, Stmt *s = 0);
			~CaseStmt();

	void	SetStmt(Stmt *s)	{ SetBody(s); }

	virtual void	EmitActual(Bytecode &b);
	virtual Stmt*	CloneActual(Mapping *b) const;

	void			EmitSwitchCases(Bytecode &b, SwitchState &s);

private:
	int		fValue;
	LexLocation fLocation;
	int		fLabel;
};


#endif
