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
#ifndef __RCX_SerialPipe_h
#define __RCX_SerialPipe_h

#ifndef __RCX_Pipe_h
#include "RCX_Pipe.h"
#endif

#ifndef __PSerial_h
#include "PSerial.h"
#endif


class RCX_SerialPipe : public RCX_Pipe
{
public:
			RCX_SerialPipe();
	virtual		~RCX_SerialPipe();

	virtual RCX_Result	Open(const char *name, int mode);
	virtual void		Close();

	virtual int		GetCapabilities() const;
	virtual RCX_Result	SetMode(int mode);

	virtual long		Read(void *ptr, long count, long timeout_ms);
	virtual long		Write(const void *ptr, long count);

private:
	PSerial*	fSerial;
};


#endif
