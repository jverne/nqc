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

#ifndef __DirList_h
#define __DirList_h

#ifndef __PListS_h
#include "PListS.h"
#endif

class DirList
{
public:

	enum
	{
		kMaxPathname = 1024
	};

			~DirList();

	void	Add(const char *dirspec);
	bool	Find(const char *filename, char *pathname);

private:
	class Entry : public PLinkS<Entry>
	{
	public:
			Entry(const char *path);
			~Entry();

	const char *	GetPath() const	{ return fPath; }

	private:
		char*	fPath;
	};

	PListS<Entry>	fEntries;
};


/*
 * the following conditions attempt to set DIR_DELIMITER base
 * on the intended target.  They may be overridden by defining
 * DIR_DELIMITER prior to compilation.
 */

#if !defined(DIR_DELIMITER)
	#if defined(macintosh)
		#define DIR_DELIMITER ':'
	#elif defined(WIN32)
		#define DIR_DELIMITER '\\'
	#else
		// assume unix
		#define DIR_DELIMITER '/'
	#endif
#endif


#endif
