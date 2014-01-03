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
#include "RCX_Pipe.h"
#include "PTypes.h"
#include "USBTowerWin.h"


#define K_WRITE_WAIT 40
#define K_USB_WRITE 0
#define K_USB_READ  20

/*
 * This file provides USB tower support by thunking over to the
 * \\.\LEGOTOWER1 device which is almost like a serial port.
 *
 */

class RCX_USBTowerPipe_win : public RCX_Pipe
{
public:
	RCX_USBTowerPipe_win();
	virtual	~RCX_USBTowerPipe_win() { Close(); }

	virtual RCX_Result	Open(const char *name, int mode);
	virtual void		Close();

	virtual int		GetCapabilities() const;
	virtual RCX_Result	SetMode(int mode);

	virtual long		Read(void *ptr, long count, long timeout_ms);
	virtual long		Write(const void *ptr, long count);
        virtual void            FlushRead(int delay);
        virtual bool            IsUSB() const { return true; };

protected:
private:
	void	SetTimeout(long);
        void    FlushWrite();
	HANDLE	fFile;
        DWORD   fLastWrite;
        bool TowerAPILoaded;
};


RCX_Pipe* RCX_NewUSBTowerPipe()
{
	return new RCX_USBTowerPipe_win();
}

#define DEFAULT_TOWER_NAME "\\\\.\\LEGOTOWER1"

RCX_USBTowerPipe_win::RCX_USBTowerPipe_win() : fFile(INVALID_HANDLE_VALUE)
{
}

RCX_Result RCX_USBTowerPipe_win::Open(const char *name, int mode)
{
        TowerAPILoaded = LoadLEGOTowerFunctions(&TOWER);
	if (name == 0 || *name==0)
	{
		name = DEFAULT_TOWER_NAME;
	}

	fFile = CreateFile(name, GENERIC_READ | GENERIC_WRITE,
		0, 0, OPEN_EXISTING, 0, 0);

	if (fFile == INVALID_HANDLE_VALUE) return kRCX_OpenSerialError;

        RCX_Result err = SetMode(mode);
        if (RCX_ERROR(err))
        {
          Close();
          return err;
        }
	return kRCX_OK;
}


void RCX_USBTowerPipe_win::Close()
{
	if (fFile == INVALID_HANDLE_VALUE) return;

	CloseHandle(fFile);
	fFile = INVALID_HANDLE_VALUE;
}


long RCX_USBTowerPipe_win::Write(const void *ptr, long count)
{
        DWORD actual = 10;

        long result;

        if (!WriteFile(fFile, ptr, count, &actual, 0))
            result = -1;
        else
            result = actual;

        FlushWrite();

        return result;
}

void RCX_USBTowerPipe_win::FlushWrite()
{
  if (TowerAPILoaded)
  {
          TOWER.Flush(fFile, LT_FLUSH_TX_BUFFER);
  }
  else
  {
        FlushFileBuffers(fFile);
  }
}

long RCX_USBTowerPipe_win::Read(void *ptr, long count, long timeout_ms)
{
	DWORD actual = 10;

	SetTimeout(timeout_ms);

	if (!ReadFile(fFile, ptr, count, &actual, 0))
	{
		GetLastError();
		DWORD Errors;
		COMSTAT cstat;
		ClearCommError( fFile, &Errors, &cstat );

		return 0;
	}

	return actual;
}


int RCX_USBTowerPipe_win::GetCapabilities() const
{
	// the ABSORB_0x55_FLAG is needed to tell the
	// transport that initial 0x55 bytes don't make
	// it through the USB/driver shim
	return kNormalIrMode + kFastIrMode + kSpyboticsMode + kFastOddParityFlag + kAbsorb55Flag;
}


RCX_Result RCX_USBTowerPipe_win::SetMode(int mode)
{
        switch(mode)
        {
          case kNormalIrMode:
            if (TowerAPILoaded)
            {
              TOWER.SetIRSpeed(fFile, LT_SPEED_BPS_2400, LT_SPEED_BPS_2400);
              return kRCX_OK;
            }
            else
              return kRCX_OK;
          case kFastIrMode:
            if (TowerAPILoaded)
            {
//              TOWER.SetRange(fFile, LT_RANGE_SHORT);
              TOWER.SetIRSpeed(fFile, LT_SPEED_BPS_4800, LT_SPEED_BPS_4800);
              TOWER.SetCarrierFrequency(fFile, 38);
              return kRCX_OK;
            }
            break;
          case kSpyboticsMode:
            if (TowerAPILoaded)
            {
//              TOWER.SetRange(fFile, LT_RANGE_SHORT);
              TOWER.SetIRSpeed(fFile, LT_SPEED_BPS_4800, LT_SPEED_BPS_4800);
              return kRCX_OK;
            }
            break;
        }
        return kRCX_PipeModeError;
}


void RCX_USBTowerPipe_win::SetTimeout(long ms)
{
	Sleep(ms / 2);
	// usb tower doesn't support SetCommTimeouts
	if (TowerAPILoaded)
	{
		TOWER.SetTimeouts(fFile, 70, K_USB_READ, K_USB_WRITE);
	}
}

void RCX_USBTowerPipe_win::FlushRead(int delay)
{
	if (TowerAPILoaded)
	{
		TOWER.Flush(fFile, LT_FLUSH_RX_BUFFER);
	}
	else
	{
		const int BUFFSIZE = 512;
		UByte * buff = new UByte[BUFFSIZE];
		while(Read(buff, BUFFSIZE, delay) > 0)
			;
		delete[] buff;
	}
}
