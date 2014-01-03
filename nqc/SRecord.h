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
#ifndef __SRecord_h
#define __SRecord_h

#ifndef __PTypes_h
#include "PTypes.h"
#endif

#include <cstdio>

using std::FILE;

class SRecord
{
public:
					SRecord() : fLength(0), fStart(0), fData(0) {}
					~SRecord()	{ delete [] fData; }

	int				GetLength() const	{ return fLength; }
	const UByte*	GetData() const		{ return fData; }
	int				GetStart() const	{ return fStart; }

	bool			Read(FILE *fp, int maxLength);

	static int		ReadHexByte(const char *ptr);

private:
	int		fLength;
	int		fStart;
	UByte*	fData;
};

#endif
