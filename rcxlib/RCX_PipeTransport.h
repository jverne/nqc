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

#ifndef __RCX_PipeTransport_h
#define __RCX_PipeTransport_h

#ifndef __RCX_Transport_h
#include "RCX_Transport.h"
#endif

#ifndef __RCX_Pipe_h
#include "RCX_Pipe.h"
#endif


class RCX_PipeTransport : public RCX_Transport
{
public:
	enum
	{
		kMinTimeout = 50,
		kMaxTimeout = 500
	};


			RCX_PipeTransport(RCX_Pipe *pipe);
	virtual		~RCX_PipeTransport();


	virtual RCX_Result 	Open(RCX_TargetType target, const char *deviceName, ULong options);
	virtual void		Close();

	virtual RCX_Result	Send(const UByte *txData, int txLength, UByte *rxData, int rxExpected, int rxMax, bool retry, int timeout);

	virtual bool		FastModeSupported() const;
	virtual bool		FastModeOddParity() const { return fPipe->GetCapabilities() & RCX_Pipe::kFastOddParityFlag; }
	virtual void		SetFastMode(bool fast);
        virtual bool            GetFastMode() const { return fFastMode; }
        virtual bool            GetComplementData() const { return fComplementData; }

	// the Receive() interface is still experiemental
	RCX_Result      Receive(UByte *data, int maxLength, bool echo);

private:
        int             ExpectedReceiveLen(const int rxExpected);
	void		BuildTxData(const UByte *data, int length, bool duplicateReduction);
	void		SendFromTxBuffer(int delay);
	RCX_Result	ReceiveReply(int rxExpected, int timeout, int &replyOffset);
	//int		ValidateRxData();  // obsolete
	int		FindReply(const int rxExpected, int &offset);
	void		CopyReply(UByte *dst, int offset, int length);
	void		AdjustTimeout(RCX_Result result, int attempt);

	void		ProcessRxByte(UByte b);
	int		VerifyReply(const int rxExpected, const UByte *data, int length, UByte cmd);

	RCX_Pipe*	fPipe;
	UByte*	        fTxData;
	int		fTxLength;
	UByte	        fTxLastCommand;

	UByte*	        fRxData;
	int		fRxLength;

	// these fields are used by the Receive() state machine
	int		fRxExpectedLength;
	int		fRxState;

	// parameters that define packet formatting
	const UByte*	fSync;	// sync pattern for packets (depends on target and usb mode)
	bool		fComplementData;

	bool		fVerbose;
	bool		fSynced;
	RCX_TargetType	fTarget;
	int		fRxTimeout;
	bool		fDynamicTimeout;

	// use for fast download
	bool		fFastMode;
};


#endif
