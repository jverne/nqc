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

#ifndef __Symbol_h
#define __Symbol_h

#ifndef __PHashTable_h
#include "PHashTable.h"
#endif

class Macro;
class Fragment;
class Stmt;

class SymbolTable;

class Symbol : public PHashable
{
public:
					Symbol(const char *name);
					~Symbol();

	bool			IsDefined() const	{ return fDefinition ? true : false; }
	Macro*			GetDefinition()		{ return fDefinition; }
	void			Define(Macro *d);
	void			Undefine();

	static Symbol*	Get(const char *name);
	static SymbolTable*	GetSymbolTable();

private:
	Macro*			fDefinition;
	static	SymbolTable*	sSymbolTable;
};


class SymbolTable : public PHashTable<Symbol>
{
public:
			SymbolTable();
			~SymbolTable();

	void	Dump();

private:
};

#endif
