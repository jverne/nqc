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
#include <cstring>
#include <cstdio>
#include "Symbol.h"
#include "Macro.h"
#include "Fragment.h"
#include "Stmt.h"

using std::printf;

#define kHashSize 1023


SymbolTable * Symbol::sSymbolTable = 0;

Symbol::Symbol(const char *name)
{
	fKey = new char[strlen(name) + 1];
	strcpy(fKey, name);
	fDefinition = 0;
}


Symbol::~Symbol()
{
	Undefine();
	delete [] fKey;
}


void Symbol::Define(Macro *d)
{
	delete fDefinition;
	fDefinition = d;
}


void Symbol::Undefine()
{
	delete fDefinition;
	fDefinition = 0;
}


Symbol *Symbol::Get(const char *name)
{
	if (!sSymbolTable)
		sSymbolTable = new SymbolTable();

	Symbol *s;

	s = sSymbolTable->Find(name);
	if (!s)
	{
		s = new Symbol(name);
		sSymbolTable->Add(s);
	}

	return s;
}


SymbolTable *Symbol::GetSymbolTable()
{
	if (!sSymbolTable)
		sSymbolTable = new SymbolTable();

	return sSymbolTable;
}


SymbolTable::SymbolTable()
	: PHashTable<Symbol>(kHashSize)
{
}


void SymbolTable::Dump()
{
	int i;
	Symbol *s;
	const Macro *d;

	for(i=0; i<GetBucketCount(); i++)
		for(s=GetBucket(i); s; s= (Symbol *)s->GetNext())
		{
			printf("%s = ", s->GetKey());
			d = s->GetDefinition();
			if (d)
			{
				printf("macro [%d]\n", d->GetArgCount());
			}
			else
			{
				printf("no def\n");
			}
		}
}


SymbolTable::~SymbolTable()
{
}
