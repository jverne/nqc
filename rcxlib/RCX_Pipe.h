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

#ifndef __RCX_Pipe_h
#define __RCX_Pipe_h

#ifndef __RCX_Result_h
#include "RCX_Result.h"
#endif

#include "PTypes.h"

class RCX_Pipe
{
public:
	// modes and flags
	enum {
		kNormalIrMode =		1 << 0,	// 2400 baud, odd parity, etc
		kFastIrMode =		1 << 1,	// 4800 baud (odd or no parity)
		kCyberMasterMode =	1 << 2,	// normal IR + DTR/RTS for CyberMaster
		kSpyboticsMode = 	1 << 3,	// 4800 baud, odd parity, DTR/RTS

		kFastOddParityFlag = 1 << 28,	// set if kFastIrMode uses odd parity
		kTxEchoFlag = 1 << 29,		// set if Tx data gets echoed
		kAbsorb55Flag =  1 << 30	// set if pipe absorbs leading 0x55
	};

        RCX_Pipe() {}
	virtual ~RCX_Pipe() {}


	virtual RCX_Result	Open(const char *name, int mode) = 0;
	virtual void		Close() = 0;

	virtual int		GetCapabilities() const = 0;
	virtual RCX_Result	SetMode(int mode) = 0;

	virtual long		Read(void *ptr, long count, long timeout_ms) = 0;
	virtual long		Write(const void *ptr, long count) = 0;
        virtual void            FlushRead(int delay);
        virtual bool            IsUSB() const { return false; }

protected:
private:
};


/*
 * This is an operating system dependent factory method for creating
 * a pipe for a USB IR tower.
 */
RCX_Pipe* RCX_NewUSBTowerPipe();

#endif
