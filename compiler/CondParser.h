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
