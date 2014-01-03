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

#ifdef PDEBUG

#include <cstdio>
#include "PDebug.h"

static const char *get_leafname(const char *filename);


void _p_assert(const char* file, int line)
{
	char buffer[512];
	sprintf(buffer, "PASSERT() failed line #%d in file %s", line, get_leafname(file));

	_p_debugstr(buffer);
}

void _p_require(const char* file, int line, const char* label)
{
	char buffer[512];
	sprintf(buffer, "PREQUIRE %s failed line #%d in file %s", label, line, get_leafname(file));

	_p_debugstr(buffer);
}


void _p_requirenot(const char* file, int line, int val, const char* label)
{
	char buffer[512];
	sprintf(buffer, "PREQUIRENOT %s failed line #%d in file %s, value = %d",
		label, line, get_leafname(file), val);

	_p_debugstr(buffer);
}


const char *get_leafname(const char *filename)
{
	const char *leaf = filename;
	const char *ptr = filename;

	while(*ptr != 0)
	{
		if (*ptr == ':')
			leaf = ptr+1;
		ptr++;
	}

	return leaf;
}

#ifndef _p_debugstr

void _p_debugstr(char *text)
{
	printf("%s\n", text);
}

#endif

#endif

