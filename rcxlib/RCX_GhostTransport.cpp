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

#include "RCX_GhostTransport.h"
#include "RCX_Link.h"
#include "PDebug.h"
#include <cstdio>
#include <CodeFragments.h>

using std::printf;

#define SESSION "LEGO.Pbk.CommStack.Session"
#define IR_PROTOCOL "LEGO.Pbk.CommStack.Protocol.IR"
#define USB_PORT "LEGO.Pbk.CommStack.Port.USB"
//#define RS232_PORT "LEGO.Pbk.CommStack.Port.RS232"


RCX_GhostTransport::RCX_GhostTransport()
{
	fOpen = false;
}


RCX_GhostTransport::~RCX_GhostTransport()
{
	Close();
}


RCX_Result RCX_GhostTransport::Open(RCX_TargetType target, const char *deviceName, ULong options)
{
#pragma unused(target, deviceName)

	// check for GhostAPI
	if ((void*)GhCreateStack == (void*)kUnresolvedCFragSymbolAddress)
		return kRCX_GhostNotFoundError;

	fVerbose = (options & RCX_Link::kVerboseMode);

	PBKRESULT err;

	err = GhCreateStack(USB_PORT, IR_PROTOCOL, SESSION, &fStack);
	PREQUIRENOT(err, Fail_GhCreateStack);

	err = GhSelectFirstDevice(fStack, 0, 0);
	PREQUIRENOT(err, Fail_GhSelectFirstDevice);

	err = GhOpen(fStack);
	PREQUIRENOT(err, Fail_GhOpen);

	// probably should check for errors...
	GhSetWaitMode(fStack, 0);
	GhGetRetries(fStack, &fExRetries, &fDownRetries);

	fOpen = true;

	return kRCX_OK;

Fail_GhOpen:
Fail_GhSelectFirstDevice:
	GhDestroyStack(fStack);

Fail_GhCreateStack:
	return kRCX_OpenSerialError;
}


void RCX_GhostTransport::Close()
{
	if (fOpen)
	{
		GhClose(fStack);
		GhDestroyStack(fStack);
		fOpen = false;
	}
}


RCX_Result RCX_GhostTransport::Send(const UByte *txData, int txLength, UByte *rxData, int rxExpected, int rxMax, bool retry)
{
	RCX_Result result = kRCX_RequestError;
	PBKRESULT err;

	GHQUEUE queue = CreateQueue(txData, txLength, rxExpected);
	PREQUIRE(queue, Fail_CreateQueue);

	if (fVerbose)
	{
		printf("Tx: ");
		DumpData(txData, txLength);
	}

	if (rxExpected == 0) retry = false;	// can't retry a one-way packet

	if (retry)
	{
		err = GhExecute(fStack, queue);
	}
	else
	{
		GhSetRetries(fStack, 0, 0);
		err = GhExecute(fStack, queue);
		GhSetRetries(fStack, fExRetries, fDownRetries);
	}

	if (err)
	{
		result = kRCX_ReplyError;
	}
	else if (rxExpected)
	{
		result = ExtractReply(queue, rxData, rxMax);
	}
	else
	{
		result = kRCX_OK;
	}

	GhDestroyCommandQueue(queue);

Fail_CreateQueue:
	return result;
}


RCX_Result RCX_GhostTransport::ExtractReply(GHQUEUE queue, UByte *rxData, int rxMax)
{
	GHCOMMAND command;
	PBKRESULT err;

	uint32 rxLength;

	err = GhGetFirstCommand(queue, &command);
	PREQUIRENOT(err, Fail_GhGetFirstCommand);

	err = GhGetCommandReplyLen(command, &rxLength);
	PREQUIRENOT(err, Fail_GhGetCommandReplyLen);

	if (rxData)
	{
		if (rxLength > rxMax) rxLength = rxMax;

		err = GhGetCommandReply(command, rxData, rxLength);
		PREQUIRENOT(err, Fail_GhGetCommandReply);
		if (fVerbose)
		{
			printf("Rx: ");
			RCX_Transport::DumpData(rxData, rxLength);
		}
	}

	return rxLength-1;

Fail_GhGetCommandReply:
Fail_GhGetCommandReplyLen:
Fail_GhGetFirstCommand:
	return kRCX_ReplyError;
}


GHQUEUE RCX_GhostTransport::CreateQueue(const UByte *txData, int txLength, int rxExpected)
{
	PBKRESULT err;
	GHQUEUE queue;

	err = GhCreateCommandQueue(&queue);
	PREQUIRENOT(err, Fail_GhCreateCommandQueue);

	err = GhAppendCommand(queue, (UByte *)txData, (ULong)txLength, (ULong)rxExpected);
	PREQUIRENOT(err, Fail_GhAppendCommand);

	return queue;

Fail_GhCreateCommandQueue:
Fail_GhAppendCommand:
	return 0;
}



