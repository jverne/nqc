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
