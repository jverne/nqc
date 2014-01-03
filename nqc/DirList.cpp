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

#include <cstring>

#if defined(__MWERKS__) && (!__MACH__)
#include <stat.h>
#else
#include <sys/stat.h>
#endif
#include "DirList.h"

using std::strcat;
using std::strlen;
using std::size_t;

#ifdef DEBUG
using std::strcpy;
#endif

DirList::~DirList()
{
	Entry *e;

	while((e=fEntries.RemoveHead()) != 0)
		delete e;
}


void DirList::Add(const char *path)
{
	// ignore NULL or empty path
	if (path==0 || *path==0) return;

	Entry *e = new Entry(path);
	fEntries.InsertTail(e);
}


bool DirList::Find(const char *filename, char *pathname)
{
	struct stat stat_buf;

	strcpy(pathname, filename);
	if (stat(pathname, &stat_buf) == 0)
		return true;

	for(Entry *e = fEntries.GetHead(); e; e=e->GetNext())
	{
		strcpy(pathname, e->GetPath());
		strcat(pathname, filename);
		if (stat(pathname, &stat_buf) == 0)
		{
			return true;
		}
	}

	return false;
}


DirList::Entry::Entry(const char *path)
{
	size_t length = strlen(path);

	fPath = new char[length+2]; // leave room for terminating delimiter
	strcpy(fPath, path);

	// should we append the delimiter?
	if (path[length-1] != DIR_DELIMITER)
	{
		fPath[length] = DIR_DELIMITER;
		fPath[length+1] = 0;
	}
}


DirList::Entry::~Entry()
{
	delete [] fPath;
}

