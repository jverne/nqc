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
