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
