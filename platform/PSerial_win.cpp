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

#include <windows.h>
#include "PSerial.h"

#define kDefaultSpeed	9600
#define kInputBufferSize	16384
#define kOutputBufferSize	16384

class PSerial_win : public PSerial
{
public:
	virtual	bool	Open(const char *name);

	virtual void	Close();
	virtual long	Read(void *ptr, long count);
	virtual long	Write(const void *ptr, long count);
	virtual void	FlushWrite();

	virtual bool	SetTimeout(long timeout_ms);

	virtual bool	SetSpeed(int speed, int opts = 0);
	virtual bool	SetDTR(bool assert);
	virtual bool	SetRTS(bool assert);

private:
	HANDLE	fFile;
        DWORD   fLastWrite;
};


PSerial * PSerial::NewSerial()
{
	return new PSerial_win;
}


const char *PSerial::GetDefaultName()
{
	return "\\\\.\\COM1";
}


bool PSerial_win::Open(const char *name)
{
        fLastWrite = GetTickCount();
	if (fOpen) return false;

	fFile = CreateFile(name,GENERIC_READ | GENERIC_WRITE,
		0, 0, OPEN_EXISTING, 0, 0);

	if (fFile == INVALID_HANDLE_VALUE) return false;

//	SetupComm(fFile, kInputBufferSize, kOutputBufferSize);

	if (!SetSpeed(kDefaultSpeed)) return false;
	SetBlocking(true);
	fOpen = true;
	return true;
}


bool PSerial_win::SetSpeed(int speed, int opts)
{
        DCB dcb;

        dcb.DCBlength = sizeof(DCB);           // sizeof(DCB)
        if (!GetCommState(fFile, &dcb)) return false;
        dcb.BaudRate = speed;
        dcb.fBinary = 1;
        dcb.fOutxCtsFlow = 0;
        dcb.fOutxDsrFlow = 0;
        dcb.fDtrControl = DTR_CONTROL_ENABLE;       // DTR flow control type
        dcb.fDsrSensitivity = 0;   // DSR sensitivity
        dcb.fTXContinueOnXoff = 0; // XOFF continues Tx
        dcb.fOutX = 0;            // XON/XOFF out flow control
        dcb.fInX = 0;             // XON/XOFF in flow control
        dcb.fErrorChar = 0;       // enable error replacement
        dcb.fNull = 0;            // enable null stripping
        dcb.fRtsControl = 0;       // RTS flow control
        dcb.fAbortOnError = 0;     // abort reads/writes on error
        dcb.XonLim = 100;
        dcb.XoffLim = 100;
        dcb.ByteSize = 8 - (opts & kPSerial_DataMask);						// number of bits/byte, 4-8
        dcb.Parity = (opts & kPSerial_ParityMask) >> kPSerial_ParityShift;	// 0-4=no,odd,even,mark,space
        dcb.StopBits = ((opts & kPSerial_StopMask) >> kPSerial_StopShift); // 0,1,2 = 1, 1.5, 2
        dcb.XonChar = 1;
        dcb.XoffChar = 2;
        dcb.ErrorChar = 0;
        dcb.EofChar = 0;
        dcb.EvtChar = 0;
        return SetCommState(fFile, &dcb);
}


bool PSerial_win::SetTimeout(long timeout_ms)
{
	COMMTIMEOUTS timeouts;

	timeouts.ReadIntervalTimeout = 0;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;

	switch(timeout_ms)
	{
		case kPStream_NeverTimeout:
			break;
		case 0:
			timeouts.ReadIntervalTimeout = MAXDWORD;
			break;
		default:
			if (GetIsSpy())
                        {
				timeouts.ReadIntervalTimeout        = MAXDWORD;
				timeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
                        }
			timeouts.ReadTotalTimeoutConstant   = timeout_ms;
			break;
	}
	SetCommTimeouts(fFile, &timeouts);
	return true;
}


void PSerial_win::Close()
{
	if (!fOpen) return;

	CloseHandle(fFile);
	fOpen = false;
}

/*
#define K_WRITE_WAIT 40
//#define min(x, y) (x > y ? y : x)
int Min(int x, int y)
{
   return x > y ? y : x;
}
*/

long PSerial_win::Write(const void *ptr, long count)
{
/*
	int sleepTime;
	sleepTime = Min(K_WRITE_WAIT, K_WRITE_WAIT - (GetTickCount()-fLastWrite));
	if (sleepTime > 0)
		Sleep(sleepTime);
*/
	DWORD actual;

	long result;

	if (!WriteFile(fFile, ptr, (unsigned long)count, &actual, (struct _OVERLAPPED *)NULL))
		result = -1;
	else
		result = actual;

//	fLastWrite = GetTickCount();
//	FlushWrite();

	return result;
}


long PSerial_win::Read(void *ptr, long count)
{
	DWORD actual;


	if (!ReadFile(fFile, ptr, count, &actual, (struct _OVERLAPPED *)NULL))
	{
		GetLastError();
		DWORD Errors;
		COMSTAT cstat;
		ClearCommError( fFile, &Errors, &cstat );

		return 0;
	}

	return actual;
}


bool PSerial_win::SetDTR(bool assert)
{
	return EscapeCommFunction(fFile, assert ? SETDTR : CLRDTR);
}


bool PSerial_win::SetRTS(bool assert)
{
	return EscapeCommFunction(fFile, assert ? SETRTS : CLRRTS);
}


void PSerial_win::FlushWrite()
{
	FlushFileBuffers(fFile);
}
