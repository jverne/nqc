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

#include "CmdLine.h"
#include <cstring>
#include <cctype>

CmdLine::~CmdLine()
{
	unsigned int i;

	for(i=0; i<fArgs.size(); ++i)
	{
		delete[] fArgs[i];
	}
}


/*
 * Add a single argument
 */

void CmdLine::Add(const char *a)
{
	char *s;

	if (!a) return;

	s = new char[strlen(a) + 1];
	strcpy(s, a);
	fArgs.push_back(s);
}


/*
 * Add an array of arguments
 */

void CmdLine::Add(int argc, const char * const *argv)
{
	int i;

	for(i=0; i<argc; ++i)
		Add(argv[i]);
}


/*
 * Parse multiple arguments from a single line.
 *
 * Use double quotes to enclose whitespace within arguments.
 * No escaping is performed.  The 'skip' parameter indicates
 * how many of the resulting arguments should be skipped (i.e.
 * not placed in the arg list).  This is useful if you want to
 * strip out the first argument (typically the command itself).
 */

void CmdLine::Parse(const char *line, int skip)
{
	if (!line) return;

	char *buf = new char[strlen(line)+1];
	char *dst = buf;
	const char *src = line;
	enum
	{
		IDLE = 0,
		READING,
		QUOTING
	} state = IDLE;


	while(true)
	{
		// read chars until no more
		char c = *src++;
		if (c == 0) break;

		if (c == '\"')
		{
			// quote
			if (state == QUOTING)
				state = READING;
			else
				state = QUOTING;
		}
		else if (c== ' ' || c== '\t')
		{
			// whitespace
			if (state == READING)
			{
				// done with argument, terminate it and add to list
				*dst = 0;
				if (skip)
					--skip;
				else
					Add(buf);

				// reset to idle state and empty arg
				state = IDLE;
				dst = buf;
			}
			else if (state == QUOTING)
				*dst++ = c;
		}
		else
		{
			if (state == IDLE)
				state = READING;
			*dst++ = c;
		}
	}

	// add last arg if any
	if (dst > buf)
	{
		*dst = 0;
		if (!skip)
			Add(buf);
	}

	delete [] buf;
}


#if 0
void CmdLine::Parse(const char *line)
{
	const char *start, *ptr;

	if (!line) return;

	ptr = line;
	while(*ptr)
	{
		// eat up leading whitespace
		for(start = ptr; *start; ++start)
			if (!isspace(*start)) break;
		if (*start == 0) break;

		if (*start == '\"')
		{
			++start;
			// find closing quote
			for(ptr = start; *ptr; ++ptr)
			{
				if (*ptr == '\\')
				{
					// char was escaped
					if (ptr[1] != 0) ++ptr;
				}
				else if (*ptr == '\"')
					break;
			}

			Add(start, ptr, true);
			if (*ptr == '\"')
				++ptr;
		}
		else
		{
			// find end of token
			for(ptr = start; *ptr; ++ptr)
				if (isspace(*ptr)) break;

			// new argument
			if (ptr > start)
				Add(start, ptr, false);
		}

	}
}


void CmdLine::Add(const char *start, const char *end, bool escape)
{
	int len = end - start;
	char *arg = new char[len + 1];
	char *dst = arg;

	while(len--)
	{
		if (escape && *start == '\\')
			++start;
		else
			*dst++ = *start++;
	}

	*dst++ = 0;
	fArgs.Append(arg);
}
#endif


