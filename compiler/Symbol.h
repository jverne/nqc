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
