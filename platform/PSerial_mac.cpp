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
 */

#include <Serial.h>
#include <cstring>
#include "PSerial.h"

#define kDefaultSpeed	9600

static bool SerialArbitrationExists(void);
static bool DriverIsOpen(ConstStr255Param driverName);
static OSErr OpenOneSerialDriver(ConstStr255Param driverName, short *refNum);


#if defined(powerc) || defined (__powerc)
/*
 * This hack is necessary because apparently Apple forgot to put
 * LMGetUnitTableEntryCount() into some old PowerPC ROMS.
 *
 * As a work-around a custom function is defined to access the low-mem
 * value directly, then a macro is used to ensure that the custom
 * function gets called instead of the real one in InterfaceLib
 */
extern pascal short MyLMGetUnitTableEntryCount();

pascal short MyLMGetUnitTableEntryCount()
{
	return *(short *)0x01D2;
}

#define LMGetUnitTableEntryCount MyLMGetUnitTableEntryCount
#endif


#define kDefaultBufferSize	16384

#define kNoTimeout	 -1

#define kSeparator ':'

class PSerial_mac : public PSerial
{
public:
	virtual	bool	Open(const char *name);

	virtual void	Close();
	virtual long	Write(const void *ptr, long count);
	virtual long	Read(void *ptr, long count);
	virtual bool	SetTimeout(long timeout_ms);
	virtual bool	SetDTR(bool assert);

	virtual bool	SetSpeed(int speed, int opts = 0);

private:
	Ptr		fInputBuffer;
	short	fInRef;
	short	fOutRef;
	long	fTimeoutTicks;
};


PSerial* PSerial::NewSerial()
{
	return new PSerial_mac();
}


const char *PSerial::GetDefaultName()
{
	return "a";
}


bool PSerial_mac::Open(const char *name)
{
	OSErr error;
	char inName[256];
	char outName[256];
	char *split;

	if (fOpen) return false;

	split = strchr(name, kSeparator);
	if (split)
	{
		memcpy(inName, name, split-name);
		inName[split-name] = 0;

		strcpy(outName, split+1);
	}
	else
	{
		outName[0] = '.';
		strcpy(outName+1, name);
		strcat(outName, "Out");
		inName[0] = '.';
		strcpy(inName+1, name);
		strcat(inName, "In");
	}

	CtoPstr(outName);
	error=OpenOneSerialDriver((Byte*)outName, &fOutRef);
	if (error) goto Fail1;

	CtoPstr(inName);
	error=OpenOneSerialDriver((Byte*)inName, &fInRef);
	if (error) goto Fail2;

	fInputBuffer = NewPtr(kDefaultBufferSize);
	if (!fInputBuffer) goto Fail3;

	SerSetBuf(fInRef, fInputBuffer, kDefaultBufferSize);
	if (!SetSpeed(kDefaultSpeed)) goto Fail4;

	fOpen = true;
	fTimeoutTicks = kNoTimeout;
	return true;

Fail4:
	DisposePtr(fInputBuffer);
Fail3:
	FSClose(fInRef);
Fail2:
	FSClose(fOutRef);
Fail1:
	return false;
}


void PSerial_mac::Close()
{
	if (!fOpen) return;

	KillIO(fInRef);
	CloseDriver(fInRef);
	KillIO(fOutRef);
	CloseDriver(fOutRef);
	DisposePtr(fInputBuffer);
	fOpen = false;
}


static const short sDataConfigs[] = { data8, data7, data6, data5 };
static const short sParityConfigs[] = {noParity, oddParity, evenParity};
static const short sStopConfigs[] = {stop10, stop15, stop20 };

bool PSerial_mac::SetSpeed(int speed, int opts)
{
	SerShk	shake;
	OSErr 	error;
	int divider;
	short config;
	int index;

	/* start with no handshaking */
	shake.fXOn = 0;
	shake.fCTS = 0;
	shake.xOn  = 0x11;
	shake.xOff = 0x13;
	shake.fInX = 0;
	shake.evts = 0;
	shake.fDTR = 0;

	// compute baud rate
	divider = ((115200 / speed) - 2) & 0x3ff;
	config = (short)divider;

	// data bits
	index = (opts & kPSerial_DataMask) >> kPSerial_DataShift;
	if (index > (sizeof(sDataConfigs)/sizeof(short))) return false;
	config |= sDataConfigs[index];

	// parity
	index = (opts & kPSerial_ParityMask) >> kPSerial_ParityShift;
	if (index > (sizeof(sParityConfigs)/sizeof(short))) return false;
	config |= sParityConfigs[index];

	// stop bits
	index = (opts & kPSerial_StopMask) >> kPSerial_StopShift;
	if (index > (sizeof(sStopConfigs)/sizeof(short))) return false;
	config |= sStopConfigs[index];


	error=SerReset(fOutRef, config);
	if (error) return false;
	error=SerHShake(fOutRef, &shake);
	if (error) return false;
	error=SerReset(fInRef, config);
	if (error) return false;
	error=SerHShake(fInRef, &shake);
	if (error) return false;

	return true;
}


long PSerial_mac::Write(const void *ptr, long count)
{
	OSErr error;

	if (!fOpen) return -1;

	error = FSWrite(fOutRef, &count, ptr);
	if (error) return -1;

	return count;
}


bool PSerial_mac::SetTimeout(long timeout_ms)
{
	if (timeout_ms == 0)
		fTimeoutTicks = 0;
	else if (timeout_ms == kPStream_NeverTimeout)
		fTimeoutTicks = kNoTimeout;
	else
		fTimeoutTicks = (timeout_ms * 3 + 49) / 50;

	return true;
}


bool PSerial_mac::SetDTR(bool assert)
{
	OSErr err = Control(fOutRef, assert ? 17 : 18, 0);
	return err == noErr;
}


long PSerial_mac::Read(void *ptr, long count)
{
	OSErr error;

	if (!fOpen) return -1;

	if (fTimeoutTicks != kNoTimeout)
	{
		long ready;
		unsigned long expire;

		// set expiration time
		expire = TickCount() + fTimeoutTicks;

		do
		{
			SerGetBuf(fInRef, &ready);	// check available

		} while(ready<count && TickCount() < expire);

		// no data?  then don't bother with FSRead, just return
		if (ready == 0) return 0;

		// only read what's required
		if (count > ready)
			count = ready;
	}

	error = FSRead(fInRef, &count, ptr);
	if (error) return -1;
	return count;
}



/* sample code from Apple on opening serial drivers */
enum {
    gestaltSerialPortArbitratorAttr = 'arb ',

        gestaltSerialPortArbitratorExists = 0
};


static OSErr OpenOneSerialDriver(ConstStr255Param driverName, short *refNum)
    // The one true way of opening a serial driver.  This routine
    // tests whether a serial port arbitrator exists.  If it does,
    // it relies on the SPA to do the right thing when OpenDriver is called.
    // If not, it uses the old mechanism, which is to walk the unit table
    // to see whether the driver is already in use by another program.
{
    OSErr err;

    if ( SerialArbitrationExists() ) {
        err = OpenDriver(driverName, refNum);
    } else {
        if ( DriverIsOpen(driverName) ) {
            err = portInUse;
        } else {
            err = OpenDriver(driverName, refNum);
        }
    }
    return err;
}

static bool SerialArbitrationExists(void)
    // Test Gestalt to see if serial arbitration exists
    // on this machine.
{
    long    response;
    OSErr	err;

    err = Gestalt(gestaltSerialPortArbitratorAttr, &response);
    if (err) return false;

    return (response & (1 << gestaltSerialPortArbitratorExists)) ? true : false;
}


static bool DriverIsOpen(ConstStr255Param driverName)
    // Walks the unit table to determine whether the
    // given driver is marked as open in the table.
    // Returns false if the driver is closed, or does
    // not exist.
{
    bool     found;
    bool     isOpen;
    short       unit;
    DCtlHandle  dceHandle;
    StringPtr   namePtr;

    found = false;
    isOpen = false;

    unit = 0;
    while ( ! found && ( unit < LMGetUnitTableEntryCount() ) ) {

        // Get handle to a device control entry.  GetDCtlEntry
        // takes a driver refNum, but we can convert between
        // a unit number and a driver refNum using bitwise not.

        dceHandle = GetDCtlEntry( (short)~unit );

        if ( dceHandle != nil && (**dceHandle).dCtlDriver != nil ) {

            // If the driver is RAM based, dCtlDriver is a handle,
            // otherwise it's a pointer.  We have to do some fancy
            // casting to handle each case.  This would be so much
            // easier to read in Pascal )-:

            if ( ((**dceHandle).dCtlFlags & dRAMBasedMask) != 0 ) {
                namePtr = & (**((DRVRHeaderHandle) (**dceHandle).dCtlDriver)).drvrName[0];
            } else {
                namePtr = & (*((DRVRHeaderPtr) (**dceHandle).dCtlDriver)).drvrName[0];
            }

            // Now that we have a pointer to the driver name, compare
            // it to the name we're looking for.  If we find it,
            // then we can test the flags to see whether it's open or
            // not.

            if ( EqualString(driverName, namePtr, false, true) ) {
                found = true;
                isOpen = ((**dceHandle).dCtlFlags & dOpenedMask) != 0;
            }
        }
        unit += 1;
    }

    return isOpen;
}


