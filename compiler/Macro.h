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

#ifndef __Macro_h
#define __Macro_h

#ifndef __Token_h
#include "Token.h"
#endif

#ifndef __PTypes_h
#include "PTypes.h"
#endif

class Macro
{
public:
	enum { kNoArgs = -1 };

			Macro(const Token *tokens, int tokenCount, int argCount);
			~Macro();

	int		GetTokenCount() const		{ return fTokenCount; }
	const Token*	GetTokens() const	{ return fTokens; }
	int		GetArgCount() const			{ return fArgCount; }

	bool	IsMarked() const	{ return fMark; }
	void	SetMark()			{ fMark = true; }
	void	ClearMark()			{ fMark = false; }

private:
	Token*	fTokens;
	int		fTokenCount;
	int		fArgCount;
	bool	fMark;
};

#endif
