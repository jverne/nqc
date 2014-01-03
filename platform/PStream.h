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

#ifndef __PStream_h
#define __PStream_h

#ifndef __PTypes_h
#include "PTypes.h"
#endif

// timeout modes
#define kPStream_NeverTimeout	-1


class PStream
{
public:
					PStream()	{ fOpen = false; }
	virtual 		~PStream()	{ Close(); }

	// Open(...) methods declared in subclasses

	virtual void	Close() { fOpen = false; }
	virtual long	Read(void *ptr, long count) = 0;
	virtual long	Write(const void *ptr, long count) = 0;

	virtual void	FlushWrite();
	virtual bool	SetTimeout(long timeout_ms = kPStream_NeverTimeout);

	virtual bool	ReadLine(char *ptr, int max);

			bool	SetBlocking(bool blocking = true);	// backwards compat. call to SetTimeout()

protected:
	bool	fOpen;
};

class PSeekStream : public PStream
{
public:
	virtual long	GetSize() const = 0;
	virtual long	GetPos() const = 0;
	virtual bool	SetPos(long pos) = 0;
};

#endif
