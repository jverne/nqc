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
#ifndef __CondParser_h
#define __CondParser_h

#ifndef __Token_h
#include "Token.h"
#endif

#ifndef __PTypes_h
#include "PTypes.h"
#endif

class Symbol;

class CondParser
{
public:
			CondParser();
			~CondParser();

	bool	Parse(long &value);

private:
	enum State
	{
		kValueState,
		kOpState
	};

	void	Init();
	bool	DoValueState(int t, TokenVal v);
	bool	DoOpState(int t);
	bool	ParseDefined(long &value);

	void	ReduceUnaries(long x);
	void	Reduce(int limit=0);

	void	Push(long x);
	long	Pop()			{ return fStack[--fDepth]; }
	void	Drop(int n)		{ fDepth -= n; }

	State	fState;
	int		fUnaryCount;
	int		fParen;
	int		fDepth;
	int		fMaxDepth;
	long*	fStack;
	Symbol*	fDefinedSymbol;
};

#endif
