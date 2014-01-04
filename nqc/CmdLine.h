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
#ifndef __CmdLine_h
#define __CmdLine_h

#include <cstdlib>
#include <vector>

using std::atoi;
using std::vector;

class CmdLine
{
public:
			CmdLine() : fPos(0) {};
			~CmdLine();

	void		Add(const char *a);	// make copy and add to arglist
	void		Add(int argc, const char * const *argv);	// add multiple args

	void		Parse(const char *line, int skip = 0);	// parse line and add args

	int			Remain() const	{ return fArgs.size() - fPos; }
	const char *Next()			{ return fArgs[fPos++]; }
	int			NextInt()		{ return atoi(Next()); }

private:
	vector<char*>	fArgs;
	int				fPos;
};

#endif
