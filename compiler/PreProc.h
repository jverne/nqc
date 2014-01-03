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

#ifndef __PreProc_h
#define __PreProc_h

#ifndef __Token_h
#include "Token.h"
#endif

#ifndef __PListS_h
#include "PListS.h"
#endif

#ifndef __CondParser_h
#include "CondParser.h"
#endif

#ifndef __Conditional_h
#include "Conditional.h"
#endif

#include <vector>

using std::vector;

class Symbol;
class Expansion;

class PreProc
{
public:
			PreProc();
			~PreProc();

	// get fully preprocessed token
	int		Get(TokenVal &v);

	// get after substitution but before directive processing
	int		GetReplacedToken(TokenVal &v);

	// get next queued token (either from macro or file)
	int		GetRawToken(TokenVal &v);

private:

	bool	DoDefine();
	bool	DoInclude();
	int		DoSimpleDefine(Symbol *s);
	int		DoMacroDefine(Symbol *s);
	bool	DoIfdef(bool b);
	bool	DoUndef();
	bool	DoPragma();

	int		ReadDefineArgs();
	void	ReadDefineBody();
	int		MatchArg(const Symbol *s);

	void	DiscardLine();

	bool	BeginExpansion(Symbol *s);
	bool	ReadExpansionArgs(Expansion *e);
	bool	ReadExpansionArg(Expansion *e, int i, int delim);

	bool	fNLRead;

	PListS<Expansion>	fExpList;
	vector<Symbol*>		fArguments;
	vector<Token>		fTokenBuf;
	Conditional			fConditional;
	bool				fActive;
	CondParser			fParser;
	bool				fEndOfFiles;
};


extern PreProc *gPreProc;

#endif
