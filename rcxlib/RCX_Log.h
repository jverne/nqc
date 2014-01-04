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
#ifndef __RCX_Log_h
#define __RCX_Log_h

#ifndef __RCX_Result_h
#include "RCX_Result.h"
#endif

#ifndef __PTypes_h
#include "PTypes.h"
#endif


class RCX_Link;

class RCX_Log
{
public:
				RCX_Log();
				~RCX_Log();

	RCX_Result	Upload(RCX_Link *link);

	void		SetLength(int length);

	int			GetLength() const	{ return fLength; }
	UByte		GetType(int index) const	{ return fTypes[index]; }
	short		GetData(int index) const	{ return fData[index]; }
	void		SPrintEntry(char *buf, int index, bool verbose) const;

private:
	int			fLength;
	UByte*		fTypes;
	short*		fData;
};


#endif
