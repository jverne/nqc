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

#ifndef __Stmt_h
#define __Stmt_h

#ifndef __PListS_h
#include "PListS.h"
#endif

#ifndef __AutoFree_h
#include "AutoFree.h"
#endif

#ifndef __LocationNode_h
#include "LocationNode.h"
#endif

#include <vector>

using std::vector;

class Bytecode;
class CheckState;
class Mapping;
class Expr;

/*
 * Stmt is the abstract base class for all program statements
 */

class Stmt :  public PLinkS<Stmt>, public AutoFree
{
public:
					Stmt();
	virtual 		~Stmt();

	void SetLocation(LocationNode *node);
	const LexLocation&	GetLoc() const { return fLoc; }

	Stmt*	GetParent() const { return fParent; }
	bool	IsDescendantOf(const Stmt* ancestor) const;

	virtual Stmt*		GetChildren() = 0;

			// flag used to force statement to be emitted (not optimized out)
	bool	GetMustEmit() const	{ return fMustEmit; }
	void	SetMustEmit(bool b)	{ fMustEmit = b; }

			// overall emit which will call the virtual EmitActual
			void	Emit(Bytecode &b);

	virtual void	EmitActual(Bytecode &b) = 0;

			Stmt*	Clone(Mapping *b) const;
	virtual Stmt*	CloneActual(Mapping *b) const = 0;

	// append sub-expressions into vector and return number added
	virtual void	GetExprs(vector<Expr*> & /* v */) const { }


protected:
	void	Adopt(Stmt *c)	{ if (c) c->fParent = this; }

	LexLocation	fLoc;
	Stmt*		fParent;
	bool		fMustEmit;
};


/*
 * LeafStmt is an abstract base class for statements that do not
 * contain any other statements as children.
 */

class LeafStmt : public Stmt
{
public:
	virtual Stmt*	GetChildren();
};


/*
 * ChainStmt is an abstract base class for statements that contain
 * a single child.
 */

class ChainStmt : public Stmt
{
public:
					ChainStmt(Stmt *s=0)	: fBody(s) { Adopt(s); }
					~ChainStmt();

	virtual Stmt*	GetChildren();

	Stmt*			GetBody()	{ return fBody; }
	const Stmt*		GetBody() const	{ return fBody; }

protected:
	void			SetBody(Stmt *s)	{ fBody=s; Adopt(s); }

private:
	Stmt*			fBody;
};


/*
 * BinaryStmt is an abstract base class for statements that contain
 * one or two sub-statements (the second one is optional).
 */

class BinaryStmt : public Stmt
{
public:
					BinaryStmt(Stmt *s1, Stmt *s2=0);
					~BinaryStmt();

	virtual Stmt*	GetChildren();

protected:
	Stmt*			GetPrimary() { return fPrimary; }
	const Stmt*		GetPrimary() const	{ return fPrimary; }
	Stmt*			GetSecondary() { return fSecondary; }
	const Stmt*		GetSecondary() const	{ return fSecondary; }

private:
	Stmt*			fPrimary;
	Stmt*			fSecondary;
};



template <class OP> void Apply(Stmt *base, OP &op)
{
	Stmt *s;

	if (!op(base)) return;

	for(s=base->GetChildren(); s; s=s->GetNext())
		Apply(s, op);
}


#ifdef DEBUG
void DumpStmt(Stmt *s, int tab=0);
#endif


#endif
