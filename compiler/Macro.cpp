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
using std::memcpy;

#include "Macro.h"


Macro::Macro(const Token *tokens, int count, int argCount)
{
	fTokenCount = count;

	if (count)
	{
		fTokens = new Token[count];
		memcpy(fTokens, tokens, sizeof(Token) * count);
	}
	else
		fTokens = nil;

	fArgCount = argCount;
	fMark = false;
}


Macro::~Macro()
{
	delete [] fTokens;
}
