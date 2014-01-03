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
