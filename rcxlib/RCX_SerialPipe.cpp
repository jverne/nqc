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
#include "RCX_SerialPipe.h"


RCX_SerialPipe::RCX_SerialPipe() : fSerial(0)
{
}


RCX_SerialPipe::~RCX_SerialPipe()
{
	Close();
}


RCX_Result RCX_SerialPipe::Open(const char *name, int mode)
{
	PSerial* s;

	s = PSerial::NewSerial();
	if (! s->Open(name))
	{
		delete s;
		return kRCX_OpenSerialError;
	}
	fSerial = s;

	RCX_Result err;
	err = SetMode(mode);
	if (RCX_ERROR(err))
	{
		Close();
		return err;
	}

	return err;
}


void RCX_SerialPipe::Close()
{
	if (!fSerial) return;

	fSerial->Close();
	delete fSerial;
	fSerial = 0;
}


int RCX_SerialPipe::GetCapabilities() const
{
	return kNormalIrMode + kFastIrMode + kCyberMasterMode + kSpyboticsMode + kTxEchoFlag;
}


RCX_Result RCX_SerialPipe::SetMode(int mode)
{
	fSerial->SetIsSpy(false);
	switch(mode)
	{
		case kNormalIrMode:
			fSerial->SetSpeed(2400, kPSerial_ParityOdd);
			return kRCX_OK;
		case kFastIrMode:
			fSerial->SetSpeed(4800);
			return kRCX_OK;
		case kCyberMasterMode:
			fSerial->SetSpeed(2400, kPSerial_ParityOdd);
			fSerial->SetDTR(true);
			fSerial->SetRTS(false);
			return kRCX_OK;
		case kSpyboticsMode:
			fSerial->SetSpeed(4800, kPSerial_ParityOdd);
			fSerial->SetDTR(true);
			fSerial->SetRTS(false);
			fSerial->SetIsSpy(true);
			return kRCX_OK;
		default:
			return kRCX_PipeModeError;
	}
}


long RCX_SerialPipe::Read(void *ptr, long count, long timeout_ms)
{
	fSerial->SetTimeout(timeout_ms);
	return fSerial->Read(ptr, count);
}


long RCX_SerialPipe::Write(const void *ptr, long count)
{
	long n = fSerial->Write(ptr, count);
        fSerial->FlushWrite();
	return n;
}
