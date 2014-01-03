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
 * Portions created by David Baum are Copyright (C) 2000 David Baum.
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
