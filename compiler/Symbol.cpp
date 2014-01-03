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
