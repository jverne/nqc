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
