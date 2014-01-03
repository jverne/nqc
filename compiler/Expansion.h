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

#ifndef __Expansion_h
#define __Expansion_h

#ifndef __Token_h
#include "Token.h"
#endif

#ifndef __PListS_h
#include "PListS.h"
#endif

class Macro;

class Expansion : public PLinkS<Expansion> {
public:
				Expansion(Macro *def);
				Expansion(const Expansion *e, int i);

				~Expansion();

	int			NextToken(TokenVal &v);
	bool		IsDone() const	{ return fTokens==fEnd; }
	int			GetArgCount() const { return fArgCount; }
	void		SetArg(int i, const Token *t, int count);

private:
	Macro*	fDef;
	const Token*	fTokens;
	const Token*	fEnd;
	int			fArgCount;
	Token**		fArgs;
	int*		fArgLengths;
};


#endif
