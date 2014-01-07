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
#include "Stmt.h"
#include "Bytecode.h"


Stmt::Stmt()
	: fParent(0), fMustEmit(false)
{
	fLoc.fIndex = kIllegalSrcIndex;
}

Stmt::~Stmt()
{
}


bool Stmt::IsDescendantOf(const Stmt *ancestor) const
{
	const Stmt *s = this;
	while(s)
	{
		if (s==ancestor) return true;
		s = s->GetParent();
	}

	return false;
}


void Stmt::SetLocation(LocationNode *node) {
	fLoc = node->GetLoc();
	delete node;
}


void Stmt::Emit(Bytecode &b)
{
	// this is where to hook general-purpose emit code that
	// applies to all statements.  For now, just add a source tag and
	// then call EmitActual() which is virtual and does all the work

	b.AddSourceTag(RCX_SourceTag::kNormal, fLoc);

	EmitActual(b);
}



Stmt *Stmt::Clone(Mapping *b) const
{
	// this is where to hook general-purpose clone code that
	// applies to all statements.

	// for now, just copy location information after using
	// CloneActual (which is virtual) to make the actual clone

	Stmt *s = CloneActual(b);
	s->fLoc = fLoc;
	return s;
}


Stmt* LeafStmt::GetChildren()
{
	return 0;
}


ChainStmt::~ChainStmt()
{
	delete fBody;
}


Stmt* ChainStmt::GetChildren()
{
	return fBody;
}


BinaryStmt::BinaryStmt(Stmt *s1, Stmt *s2) :
	fPrimary(s1), fSecondary(s2)
{
	Adopt(s1);
	Adopt(s2);

	if (s2)
	{
		// this is a hack to get fPrimary and fSecondary linked together
		PListSS<Stmt> list;

		list.InsertHead(s2);
		list.InsertHead(s1);
	}
}


BinaryStmt::~BinaryStmt()
{
	delete fPrimary;
	delete fSecondary;
}


Stmt *BinaryStmt::GetChildren()
{
	return fPrimary;
}


#ifdef DEBUG
#include <cstdio>
#include <typeinfo>

using std::printf;
using std::putchar;

void DumpStmt(Stmt *s, int tab)
{
	for(int i=0; i<tab; i++)
		putchar(' ');
	printf("%s  %08x\n", typeid(*s).name(), (unsigned)s);

	for(Stmt*c = s->GetChildren(); c; c=c->GetNext())
	{
		DumpStmt(c, tab + 2);
	}
}
#endif
