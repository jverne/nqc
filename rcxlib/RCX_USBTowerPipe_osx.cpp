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
extern "C"
{
#include <mach/mach.h>
}

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/IOReturn.h>
#include <unistd.h>
#include "RCX_Pipe.h"

#include "PDebug.h"

#define kVendorID 1684
#define kProductID 1
#define kConfiguration 0
#define kReadPipe 1
#define kWritePipe 2

// these should come from a LEGO include
#define LTW_REQ_GET_PARM	1
#define LTW_REQ_SET_PARM	2
#define LTW_PARM_RANGE		2
#define LTW_RANGE_SHORT		1
#define LTW_RANGE_MEDIUM	2


#define LTW_REQ_SET_TX_SPEED	0xEF
#define LTW_REQ_SET_RX_SPEED	0xF1

#define SPEED_COMM_BAUD_2400	8
#define SPEED_COMM_BAUD_4800	0x10

#define LTW_REQ_SET_TX_CARRIER_FREQUENCY	0xF4



typedef struct LTW_REQ_REPLY_HEADER
	{ UInt16 wNoOfBytes;						// Number of bytes in the reply
	  UInt8 bErrCode;						// Request return code
	  UInt8 bValue;							// Request return value
} LTW_REQ_REPLY_HEADER;

typedef LTW_REQ_REPLY_HEADER LTW_REQ_GET_SET_PARM_REPLY;


static IOReturn GetInterfaceForService(io_service_t service, CFUUIDRef clientID, CFUUIDRef interfaceID, void** interface);

template <class T> static IOReturn GetInterfaceForService(io_service_t service, CFUUIDRef clientID, CFUUIDRef interfaceID, T*** intf)
{
	return GetInterfaceForService(service, clientID, interfaceID, (void**)intf);
}


class RCX_USBTowerPipe_osx : public RCX_Pipe
{
public:
		RCX_USBTowerPipe_osx();
		~RCX_USBTowerPipe_osx()	{ Close(); }

	virtual RCX_Result	Open(const char *name, int mode);
	virtual void		Close();

	virtual int		GetCapabilities() const;
	virtual RCX_Result	SetMode(int mode);

	virtual long		Read(void *ptr, long count, long timeout_ms);
	virtual long		Write(const void *ptr, long count);
        virtual bool            IsUSB() const { return true; };

private:
	enum
	{
		kReadPacketSize = 8
	};

	static void ReadCompletionGlue(void *refCon, IOReturn result, void *arg0);
	void		ReadCompletion(IOReturn result, UInt32 n);
	void		StartRead();
	void		ConsumeInBuffer();
	void		SetTimeout(long timeout_ms);

	IOReturn	OpenDevice(short vendorID, short productID);
	IOReturn	Configure(int config);
	IOReturn	OpenInterface();
	IOReturn	ControlRequest(UInt8 request, UInt16 value);
	IOReturn	ControlRequest(UInt8 request, UInt8 loByte, UInt8 hiByte) {
					return ControlRequest(request, loByte + (hiByte << 8));
				}

	IOUSBDeviceInterface**		fDevice;
	IOUSBInterfaceInterface182**	fInterface;

	// buffered read data (sometimes more data is read from the endpoint than is needed)
	unsigned char	fInBuffer[kReadPacketSize];
	unsigned char*	fInBufferStart;
	unsigned char*	fInBufferEnd;

	// the pending read request
	unsigned char*	fReadPtr;
	int		fReadRemain;
	bool		fReadDone;
};


RCX_Pipe* RCX_NewUSBTowerPipe()
{
	return new RCX_USBTowerPipe_osx();
}


RCX_USBTowerPipe_osx::RCX_USBTowerPipe_osx()
	: fDevice(0), fInterface(0)
{
}


RCX_Result RCX_USBTowerPipe_osx::Open(const char *name, int mode)
{
	IOReturn err;
        RCX_Result err2;

	err = OpenDevice(kVendorID, kProductID);
	PREQUIRENOT(err, Fail_OpenDevice);

	err = Configure(kConfiguration);
	PREQUIRENOT(err, Fail_Configure);

	err = OpenInterface();
	PREQUIRENOT(err, Fail_OpenInterface);

	UInt8 range;
	range = (strcmp(name, "short")==0) ? LTW_RANGE_SHORT : LTW_RANGE_MEDIUM;
	ControlRequest(LTW_REQ_SET_PARM, LTW_PARM_RANGE,range);

	// clear the input buffer
	fInBufferStart = fInBufferEnd = fInBuffer;

	err2 = SetMode(mode);
	if (err2 != kRCX_OK)
	{
		Close();
		return err2;
	}
	return kRCX_OK;

Fail_OpenInterface:
Fail_Configure:
Fail_OpenDevice:
	Close();
	return kRCX_OpenSerialError;
}


void RCX_USBTowerPipe_osx::Close()
{
	if (fInterface)
	{
		(*fInterface)->USBInterfaceClose(fInterface);
		(*fInterface)->Release(fInterface);
		fInterface = 0;
	}

	if (fDevice)
	{
		(*fDevice)->USBDeviceClose(fDevice);
		(*fDevice)->Release(fDevice);
		fDevice = 0;
	}
}


int RCX_USBTowerPipe_osx::GetCapabilities() const
{
	return kNormalIrMode | kFastIrMode | kSpyboticsMode | kFastOddParityFlag | kAbsorb55Flag;
}


RCX_Result RCX_USBTowerPipe_osx::SetMode(int mode)
{
	switch(mode)
	{
		case kNormalIrMode:
			ControlRequest(LTW_REQ_SET_TX_SPEED, SPEED_COMM_BAUD_2400);
			ControlRequest(LTW_REQ_SET_RX_SPEED, SPEED_COMM_BAUD_2400);
			return kRCX_OK;
		case kFastIrMode:
			ControlRequest(LTW_REQ_SET_PARM, LTW_PARM_RANGE, LTW_RANGE_SHORT);
			ControlRequest(LTW_REQ_SET_TX_SPEED, SPEED_COMM_BAUD_4800);
			ControlRequest(LTW_REQ_SET_RX_SPEED, SPEED_COMM_BAUD_4800);
			ControlRequest(LTW_REQ_SET_TX_CARRIER_FREQUENCY, 38);
			return kRCX_OK;
                case kSpyboticsMode:
                        ControlRequest(LTW_REQ_SET_PARM, LTW_PARM_RANGE, LTW_RANGE_SHORT);
                        ControlRequest(LTW_REQ_SET_TX_SPEED, SPEED_COMM_BAUD_4800);
                        ControlRequest(LTW_REQ_SET_RX_SPEED, SPEED_COMM_BAUD_4800);
                        return kRCX_OK;
		default:
			return kRCX_PipeModeError;
	}
}


#define MAX_PACKET 200

long RCX_USBTowerPipe_osx::Write(const void *ptr, long length)
{
	const unsigned char *data = (const unsigned char *)ptr;

	int total = 0;

	while(length > 0)
	{
		IOReturn err;
		int count = length;
		if (count > MAX_PACKET) count = MAX_PACKET;
		err = (*fInterface)->WritePipe(fInterface, kWritePipe, (void*)data, count);
		PREQUIRENOT(err, Fail_WritePipe);

		length -= count;
		data += count;
		total += count;
	}

Fail_WritePipe:
	return total;
}

void RCX_USBTowerPipe_osx::SetTimeout(long timeout_ms)
{
	usleep(timeout_ms * 1000 / 2);
}


long RCX_USBTowerPipe_osx::Read(void *data, long length, long timeout_ms)
{
	fReadPtr = (unsigned char *)data;
	fReadRemain = length;
	fReadDone = false;

	//SetTimeout(timeout_ms);

	// consume any previously buffered data
	ConsumeInBuffer();

	if (!fReadDone)
	{
		StartRead();
		do
		{
			SInt32 reason = CFRunLoopRunInMode(kCFRunLoopDefaultMode, timeout_ms / 1000.0, true);
			if (reason == kCFRunLoopRunTimedOut)
			{
				(*fInterface)->AbortPipe(fInterface, kReadPipe);
				fReadDone = true;
			}
		} while(!fReadDone);
	}

	return fReadPtr - (unsigned char *)data;
}


void RCX_USBTowerPipe_osx::ConsumeInBuffer()
{
	while(fReadRemain && (fInBufferStart < fInBufferEnd))
	{
		*fReadPtr++ = *fInBufferStart++;
		fReadRemain--;
	}

	if (fReadRemain==0) fReadDone = true;
}


void RCX_USBTowerPipe_osx::ReadCompletion(IOReturn result, UInt32 n)
{
	if (result == kIOReturnAborted) return;

	PREQUIRENOT(result, Fail_ReadCompletion);

	// consume input buffer
	fInBufferEnd = fInBuffer + n;
	ConsumeInBuffer();

	if (!fReadDone)
	{
		StartRead();
	}

	return;

Fail_ReadCompletion:
	fReadDone = true;
}


void RCX_USBTowerPipe_osx::StartRead()
{
	// clear the input buffer
	fInBufferStart = fInBufferEnd = fInBuffer;

	IOReturn err = (*fInterface)->ReadPipeAsync(fInterface, kReadPipe, fInBuffer, kReadPacketSize, ReadCompletionGlue ,this);
	PREQUIRENOT(err, Fail_ReadPipe);
	return;

Fail_ReadPipe:
	fReadDone = true;
	return;
}


void RCX_USBTowerPipe_osx::ReadCompletionGlue(void *refCon, IOReturn result, void *arg0)
{
	RCX_USBTowerPipe_osx* pipe = (RCX_USBTowerPipe_osx*)refCon;
	pipe->ReadCompletion(result, (size_t)arg0);
}


IOReturn RCX_USBTowerPipe_osx::OpenDevice(short vendorID, short productID)
{
    mach_port_t 		masterPort;
    CFMutableDictionaryRef 	matchingDict;
    kern_return_t		err;
	io_object_t usbDevice;

    // first create a master_port for my task
    err = IOMasterPort(MACH_PORT_NULL, &masterPort);
	PREQUIRENOT(err, Fail_IOMasterPort);
	//PREQUIRE(masterPort, Fail_IOMasterPort);

    // Set up the matching criteria for the devices we're interested in
    matchingDict = IOServiceMatching(kIOUSBDeviceClassName);	// Interested in instances of class IOUSBDevice and its subclasses
	//PREQUIRE(matchingDict, Fail_IOServiceMatching);

    CFDictionarySetValue(matchingDict, CFSTR(kUSBVendorID),
		CFNumberCreate(kCFAllocatorDefault, kCFNumberShortType, &vendorID));
    CFDictionarySetValue(matchingDict, CFSTR(kUSBProductID),
		CFNumberCreate(kCFAllocatorDefault, kCFNumberShortType, &productID));

    io_iterator_t iter;
    err = IOServiceGetMatchingServices(masterPort, matchingDict, &iter);
	mach_port_deallocate(mach_task_self(), masterPort);
	PREQUIRENOT(err, Fail_IOServiceGetMatchingServices);

	err = -1;
    usbDevice = IOIteratorNext(iter);
	IOObjectRelease(iter);
	PREQUIRE(usbDevice, Fail_NoMatch);

	err = GetInterfaceForService(usbDevice,
		kIOUSBDeviceUserClientTypeID, kIOUSBDeviceInterfaceID, &fDevice);
	IOObjectRelease(usbDevice);
	PREQUIRENOT(err, Fail_CreateInterface);

	// need to open the device in order to change its state
	err = (*fDevice)->USBDeviceOpen(fDevice);
	PREQUIRENOT(err, Fail_USBDeviceOpen);


	return 0;

Fail_USBDeviceOpen:
	(*fDevice)->Release(fDevice);
	fDevice = 0;
Fail_CreateInterface:
Fail_NoMatch:
Fail_IOServiceGetMatchingServices:
//Fail_IOServiceMatching:
Fail_IOMasterPort:
	return err;
}


IOReturn RCX_USBTowerPipe_osx::OpenInterface()
{
	IOReturn err;
	IOUSBFindInterfaceRequest request;
	io_iterator_t		iterator;
	io_service_t		usbInterface;

    request.bInterfaceClass = kIOUSBFindInterfaceDontCare;
    request.bInterfaceSubClass = kIOUSBFindInterfaceDontCare;
    request.bInterfaceProtocol = kIOUSBFindInterfaceDontCare;
    request.bAlternateSetting = kIOUSBFindInterfaceDontCare;

    err = (*fDevice)->CreateInterfaceIterator(fDevice, &request, &iterator);
	PREQUIRENOT(err, Fail_IterateInterface);

	usbInterface = IOIteratorNext(iterator);
	IOObjectRelease(iterator);
	//PREQUIRE(usbInterface, Fail_NoInterface);

	err = GetInterfaceForService(usbInterface,
		kIOUSBInterfaceUserClientTypeID, kIOUSBInterfaceInterfaceID, &fInterface);
	IOObjectRelease(usbInterface);
	PREQUIRENOT(err, Fail_GetInterface);

	err = (*fInterface)->USBInterfaceOpen(fInterface);
	PREQUIRENOT(err, Fail_OpenInterface);

	CFRunLoopSourceRef source;
	err = (*fInterface)->CreateInterfaceAsyncEventSource(fInterface, &source);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), source, kCFRunLoopDefaultMode);

	return 0;

Fail_OpenInterface:
	(void) (*fInterface)->Release(fInterface);
	fInterface  = 0;
Fail_GetInterface:
Fail_IterateInterface:
	return err;
}


IOReturn RCX_USBTowerPipe_osx::Configure(int index)
{
	IOReturn err;
    IOUSBConfigurationDescriptorPtr	confDesc;

    // get the configuration descriptor
	err = (*fDevice)->GetConfigurationDescriptorPtr(fDevice, index, &confDesc);
	PREQUIRENOT(err, Fail_GetConfiguration);

    err = (*fDevice)->SetConfiguration(fDevice, confDesc->bConfigurationValue);
	PREQUIRENOT(err, Fail_SetConfiguration);

	return 0;

Fail_SetConfiguration:
Fail_GetConfiguration:
	return err;
}


IOReturn RCX_USBTowerPipe_osx::ControlRequest(UInt8 request, UInt16 value)
{
	IOReturn err;
	IOUSBDevRequestTO req;
	LTW_REQ_GET_SET_PARM_REPLY reply;

	req.bmRequestType = (kUSBVendor<<kUSBRqTypeShift) + (kUSBIn<<kUSBRqDirnShift) + kUSBDevice;
	req.bRequest = request;
	req.wValue = value;
	req.wIndex = 0;
	req.wLength = sizeof(reply);
	req.pData = &reply;
	req.noDataTimeout = kUSBDefaultControlNoDataTimeoutMS;
	req.completionTimeout = 0;

	err = (*fInterface)->ControlRequestTO(fInterface, 0, &req);

	// size of actual reply in req.wLenDone
	//printf("%d %d %d\n", reply.wNoOfBytes, reply.bErrCode, reply.bValue);

	return err;
}


IOReturn GetInterfaceForService(io_service_t service, CFUUIDRef clientID, CFUUIDRef interfaceID, void** interface)
{
	IOCFPlugInInterface **plugInInterface;
	IOReturn err;
	SInt32 score;

	err = IOCreatePlugInInterfaceForService(service,
		clientID, kIOCFPlugInInterfaceID, &plugInInterface, &score);
	PREQUIRENOT(err, Fail_CreatePlugin);

	// I have the device plugin, I need the device interface
	err = (*plugInInterface)->QueryInterface(plugInInterface,
		CFUUIDGetUUIDBytes(interfaceID), interface);
	(*plugInInterface)->Release(plugInInterface);			// done with this
	PREQUIRENOT(err, Fail_QueryInterface);

	return 0;

Fail_QueryInterface:
Fail_CreatePlugin:
	*interface = 0;
	return err;
}
