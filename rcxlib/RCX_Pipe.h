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
