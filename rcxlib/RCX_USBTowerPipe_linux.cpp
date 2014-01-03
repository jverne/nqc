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
 * Code adapted for use with Linux LegoUSB driver by Bret Thaeler
 * 	& the LegoUSB DevTeam <http://legousb.sourceforge.net>.
 * 	LegoUSB Portions Copyright (c) 2001 - 2002 Bret Thaeler & The LegoUSB DevTeam
 *
 * The Initial Developer of this code is David Baum.
 * Portions created by David Baum are Copyright (C) 1998 David Baum.
 * All Rights Reserved.
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "RCX_Pipe.h"

#include <LegoUSB/legousbtower.h>


class RCX_USBTowerPipe_linux : public RCX_Pipe
{
public:
	        RCX_USBTowerPipe_linux() : fd(-1) {}
        virtual	~RCX_USBTowerPipe_linux() { Close(); }

	virtual RCX_Result	Open(const char *name, int mode);
	virtual void		Close();

	virtual int		GetCapabilities() const;
	virtual RCX_Result	SetMode(int mode);

	virtual long		Read(void *ptr, long count, long timeout_ms);
	virtual long		Write(const void *ptr, long count);
        virtual bool            IsUSB() const { return true; };

private:
	void		SetTimeout(long timeout_ms);
	int		fd;
};


RCX_Pipe* RCX_NewUSBTowerPipe()
{
	return new RCX_USBTowerPipe_linux();
}

#define DEFAULT_TOWER_NAME "/dev/usb/lego0"

RCX_Result RCX_USBTowerPipe_linux::Open(const char *name, int mode)
{
	// if (name == 0 || *name==0)
	{
		name = DEFAULT_TOWER_NAME;
	}

	fd = open(name, O_RDWR);

	if (fd < 0) return kRCX_OpenSerialError;

        RCX_Result err = SetMode(mode);
        if (err != kRCX_OK)
        {
          Close();
          return err;
        }
        return kRCX_OK;
}


void RCX_USBTowerPipe_linux::Close()
{
	if (fd < 0) return;

	close(fd);
	fd = -1;
}


long RCX_USBTowerPipe_linux::Write(const void *ptr, long count)
{
	ssize_t actual;
	ssize_t i;

	actual = 0;
	while(count > 0) {
		if ((i = write(fd, (void *) &((char *)ptr)[actual], (size_t)count)) < 0)
			return -1;
		count -= i;
		actual += i;
	}

	return actual;
}


long RCX_USBTowerPipe_linux::Read(void *ptr, long count, long timeout_ms)
{
	ssize_t actual;

	SetTimeout(timeout_ms);

	if ((actual = read(fd, ptr, count)) < 0)
	{
		return 0;
	}

	return actual;
}


int RCX_USBTowerPipe_linux::GetCapabilities() const
{
	// only normal IR mode is supported, and
	// the ABSORB_0x55_FLAG is needed to tell the
	// transport that initial 0x55 bytes don't make
	// it through the USB/driver shim
	// return kNormalIrMode + kAbsorb55Flag;
	return kNormalIrMode + kFastIrMode + kSpyboticsMode;
}


RCX_Result RCX_USBTowerPipe_linux::SetMode(int mode)
{
	switch(mode)
	{
		case kNormalIrMode:
                case kFastIrMode:
                case kSpyboticsMode:
			return kRCX_OK;
		default:
			return kRCX_PipeModeError;
	}
}


void RCX_USBTowerPipe_linux::SetTimeout(long timeout_ms)
{
	usleep(timeout_ms * 1000 / 2);

	if (timeout_ms == 0) {
		timeout_ms = 20;
	}

	ioctl(fd, LEGO_TOWER_SET_READ_TIMEOUT, timeout_ms);
}

