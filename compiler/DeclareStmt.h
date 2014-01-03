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
 * Portions created by David Baum are Copyright (C) 2000 David Baum.
 * All Rights Reserved.
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */

#ifndef __DeclareStmt_h
#define __DeclareStmt_h

#ifndef __Stmt_h
#include "Stmt.h"
#endif

#ifndef __LexLocation_h
#include "LexLocation.h"
#endif

class Symbol;
class Expr;
class ScopeStmt;

class DeclareStmt : public ChainStmt
{
public:
	DeclareStmt(const Symbol *name, int var, const struct LexLocation &loc, int count, bool ptr, bool stack);
	~DeclareStmt();

	void			SetInitialValue(Expr *e);

	int			GetVar() const		{ return fVar; }
	const LexLocation&	GetLoc() const	        { return fLoc; }
	int			GetCount() const	{ return fCount; }
	const Symbol*	        GetName() const		{ return fName; }
        bool                    GetPointer() const      { return fPtr; }
        bool                    GetStack() const        { return fStack; }

	void			EmitActual(Bytecode &b);
	Stmt*			CloneActual(Mapping *b) const;

	// Binder is a function object that binds declarations to a scope
	class Binder
	{
	public:
		Binder(ScopeStmt *scope) : fScope(scope) {}
		bool	operator()(Stmt *s);

	private:
		ScopeStmt*	fScope;
	};

	friend class Binder;

private:
	const Symbol*	        fName;
	int			fVar;
	ScopeStmt*		fScope;
	struct LexLocation      fLoc;
	int			fCount;
        bool                    fPtr;
        bool                    fStack;
};


#endif
