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
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "RCX_PipeTransport.h"
#include "RCX_Link.h"
#include "RCX_SerialPipe.h"

using std::printf;

#ifdef DEBUG
#define DEBUG_TIMEOUT
#endif

#define kMaxTxData  ((int)(2*RCX_Link::kMaxCmdLength + 6))
#define kMaxRxData  ((int)(kMaxTxData + 2*RCX_Link::kMaxReplyLength + 5))

#define kDefaultRetryCount 4

static UByte cmSync[] = { 1, 0xff };
static UByte rcxSync[] = { 3, 0x55, 0xff, 0x00 };
static UByte rcxNo55Sync[] = { 2, 0xff, 0x00 };
static UByte spyboticsSync[] = {1, 0x98 };

static int FindSync(const UByte *data, int length, const UByte *sync, const UByte cmd);
static UByte ComputeChecksum(UByte dataSum, RCX_TargetType targetType);

/// receive states
enum {
    kReplyState = 0,    // state when rx buffer used for replies
    kIdleState,
    kSync1State,
    kSync2State,
    kDataState
};


RCX_PipeTransport::RCX_PipeTransport(RCX_Pipe *pipe) : fPipe(pipe)
{
    fTxData = new UByte[kMaxTxData];
    fRxData = new UByte[kMaxRxData];
    fVerbose = false;
    fTxLastCommand = 0;
    fFastMode = false;
    fOmitHeader = false;
}


RCX_PipeTransport::~RCX_PipeTransport()
{
    Close();
    delete [] fTxData;
    delete [] fRxData;
    delete fPipe;
}


RCX_Result RCX_PipeTransport::Open(RCX_TargetType target, const char *deviceName, ULong options)
{
    RCX_Result err;

    fTarget = target;
    fComplementData = true; // default

    int mode;
    switch(fTarget) {
        case kRCX_CMTarget:
            mode = RCX_Pipe::kCyberMasterMode;
            fSync = cmSync;
            break;
        case kRCX_SpyboticsTarget:
            mode = RCX_Pipe::kSpyboticsMode;
            fSync = spyboticsSync;
            fComplementData = false;
            break;
        default:
            mode = RCX_Pipe::kNormalIrMode;
            fSync = (fPipe->GetCapabilities() & RCX_Pipe::kAbsorb55Flag) ? rcxNo55Sync : rcxSync;
            break;
    }

    err = fPipe->Open(deviceName, mode);
    if (RCX_ERROR(err)) return err;

    fVerbose = (options & RCX_Link::kVerboseMode);

    fRxTimeout = (options & RCX_Link::kRxTimeoutMask);
    if (fRxTimeout==0) fRxTimeout = kMaxTimeout;

    fDynamicTimeout = true;

    fRxState = kReplyState;
    return kRCX_OK;
}


void RCX_PipeTransport::Close()
{
    fPipe->Close();
}


bool RCX_PipeTransport::FastModeSupported() const
{
    return (fPipe->GetCapabilities() & RCX_Pipe::kFastIrMode);
}


void RCX_PipeTransport::SetFastMode(bool fast)
{
    if (fast == fFastMode) return;

    if (fast) {
        fComplementData = false;
        fPipe->SetMode(RCX_Pipe::kFastIrMode);
    }
    else {
        fComplementData = (fTarget != kRCX_SpyboticsTarget);
        fPipe->SetMode(RCX_Pipe::kNormalIrMode);
    }

    fFastMode = fast;
}


RCX_Result RCX_PipeTransport::Send(const UByte *txData, int txLength, UByte *rxData,
    int rxExpected, int rxMax, bool retry, int timeout)
{
    RCX_Result result;

    // format the command
    BuildTxData(txData, txLength, retry);

    // try sending
    int tries = retry ? kDefaultRetryCount : 1;
    int originalTimeout = fRxTimeout;

    for (int i=0; i<tries; i++) {
        SendFromTxBuffer(fFastMode ? 100 : 0);

        // if no reply is expected, we can just return now (no retries, no errors, etc)
        if (!rxExpected) return kRCX_OK;

        int tmpTimeout;
        if (timeout > 0)
            tmpTimeout = timeout;
        else
            tmpTimeout = fRxTimeout;

        int replyOffset;
        result = ReceiveReply(rxExpected, tmpTimeout, replyOffset);
        if (fDynamicTimeout) {
            AdjustTimeout(result, i);
        }

        if (!RCX_ERROR(result)) {
            if (rxData) {
                int length = result+1;
                if (length > rxMax) length = rxMax;
                CopyReply(rxData, replyOffset, length);
            }

            return result;
        }

        // only the second kRCX_IREchoError is catastrophic
        // this is somewhat of a hack - I really should keep track
        // of the echo, but for now this makes sure that a serial
        // level failure on a single packet doesn't kill the entire
        // send
        if (result == kRCX_IREchoError && i > 0) break;
                
        if (fVerbose) printf("Retrying...\n");
    }

    if (retry) {
        // retries exceeded, restore original timeout and lose the sync
        if (fDynamicTimeout) {
            fRxTimeout = originalTimeout;
        }
        fSynced = false;
    }

    return result;
}


void RCX_PipeTransport::BuildTxData(const UByte *data, int length, bool duplicateReduction)
{
    int i;
    UByte dataSum = 0;
    UByte byte;
    UByte *ptr = fTxData;

    if (fTarget == kRCX_CMTarget) {
        // CM sync pattern
        *ptr++ = 0xfe;
        *ptr++ = 0x00;
        *ptr++ = 0x00;
        *ptr++ = 0xff;
    }
    else if (fTarget == kRCX_SpyboticsTarget) {
        *ptr++ = 0x98;
    }
    else if (fFastMode) {
        *ptr++ = 0xff;
    }
    else {
        if (!fOmitHeader) {
            // RCX sync pattern
            *ptr++ = 0x55;
            *ptr++ = 0xff;
            *ptr++ = 0x00;
        }
    }

    // interleaved data & inverse data
    for (i=0; i<length; i++) {
        byte = *data++;

        if (i==0) {
            if (duplicateReduction && byte==fTxLastCommand)
                byte ^= 8;
            fTxLastCommand = byte;
        }


        *ptr++ = byte;
        if (fComplementData)
            *ptr++ = (UByte)~byte;
        dataSum += byte;
    }

    UByte checksum = ComputeChecksum(dataSum, fTarget);

    // checksum
    *ptr++ = checksum;
    if (fComplementData)
        *ptr++ = (UByte)~checksum;

    fTxLength = ptr - fTxData;
}


void RCX_PipeTransport::SendFromTxBuffer(int delay)
{
    // drain serial rx buffer
        fPipe->FlushRead(delay);

    // send command
    fPipe->Write(fTxData, fTxLength);
    if (fVerbose) {
        printf("Tx: ");
        DumpData(fTxData, fTxLength);
    }
}


RCX_Result RCX_PipeTransport::ReceiveReply(int rxExpected, int timeout, int &replyOffset)
{
    int receiveLen = ExpectedReceiveLen(rxExpected);
    if (!((fTarget == kRCX_SpyboticsTarget) || fPipe->IsUSB())) {
        receiveLen += fTxLength; // serial tower echoes the sent bytes
    }

    // get the reply
    fRxState = kReplyState;
    fRxLength = 0;
    int length = 0;
    bool bFirstRead = true;
    while (fRxLength < kMaxRxData) {
        if (bFirstRead) {
            bFirstRead = false;
            if (fVerbose) printf("expecting %d bytes, timeout = %d\n", receiveLen, timeout);

            int bytesRead = fPipe->Read(fRxData+fRxLength, receiveLen, timeout);
            if (bytesRead == 0) {
                break;
            }

            fRxLength += bytesRead;
            if (fVerbose) printf("read %d bytes, total = %d\n", bytesRead, fRxLength);
        }
        else {
            if (fVerbose) printf("expecting 1 byte, timeout = %d\n", timeout);

            if (fPipe->Read(fRxData+fRxLength, 1, timeout) != 1) {
                break;
            }

            fRxLength++;
            if (fVerbose) printf("read 1 byte, total = %d\n", fRxLength);
        }

        // check for replies
        length = FindReply(rxExpected, replyOffset);
        if (length == rxExpected) {
            break;
        }
    }

    if (fVerbose) {
        printf("Rx: ");
        DumpData(fRxData, fRxLength);
    }

    if (fRxLength == 0
        && (fPipe->GetCapabilities() & RCX_Pipe::kTxEchoFlag)
        && (fTarget != kRCX_CMTarget))
    {
        return  kRCX_IREchoError;
    }

    if (length == 0) {
        return kRCX_ReplyError;
    }

    return length - 1;
}

int RCX_PipeTransport::FindReply(const int rxExpected, int &offset)
{
    int length;

    offset = 0;
    while (1) {
        int start = FindSync(fRxData + offset, fRxLength - offset, fSync, fTxLastCommand);

        if (start == 0) {
            return 0;
        }

        offset += start;

        length = VerifyReply(rxExpected, fRxData + offset, fRxLength - offset, fTxLastCommand);

        if (length > 0) {
            return length;
        }
    }
}


void RCX_PipeTransport::CopyReply(UByte *dst, int offset, RCX_Result length)
{
    const UByte *src = fRxData + offset;

    while (length>0) {
        *dst++ = *src++;
        if (fComplementData) src++;
        length--;
    }
}


void RCX_PipeTransport::AdjustTimeout(RCX_Result result, int attempt)
{
    int newTimeout = fRxTimeout;

    if (!RCX_ERROR(result) && attempt==0) {
        // worked on first try, lets see if we can go faster next time
        newTimeout = fRxTimeout - (fRxTimeout / 10);
        if (newTimeout < kMinTimeout)
            newTimeout = kMinTimeout;
    }
    else if (RCX_ERROR(result) && attempt > 0) {
        // failed on try other than first - slow down
        newTimeout *= 2;
        if (newTimeout > kMaxTimeout) newTimeout = kMaxTimeout;
    }

    if (newTimeout != fRxTimeout) {
        fRxTimeout = newTimeout;
        #ifdef DEBUG_TIMEOUT
            printf("Timeout = %d\n", fRxTimeout);
        #endif
    }
}


RCX_Result RCX_PipeTransport::Receive(UByte *data, int maxLength, bool /* echo */)
{
    if (fRxState == kReplyState) fRxState = kIdleState;

    while (true) {
        UByte b;

        if (fPipe->Read(&b, 1, 100) != 1) return 0;

//      if (echo) fPipe->Write(&b, 1);
        ProcessRxByte(b);

        if (fRxState == kDataState && fRxLength>3 && (fRxLength & 1)==0) {
            // compute checksum
            int count = fRxLength/2 - 1;
            UByte checksum = 0;

            for (int i=0; i<count; ++i) {
                checksum += fRxData[i*2];
            }

            if (fRxData[count*2] != checksum) continue;

            if (count > maxLength) count = maxLength;
            for (int i=0; i<count; ++i) {
                data[i] = fRxData[i*2];
            }

            fRxState = kIdleState;
            return count;
        }
    }
}



void RCX_PipeTransport::ProcessRxByte(UByte b)
{
    int newState = fRxState;
    bool reset = false;

    if (fVerbose) {
        DumpData(&b, 1);
    }

    switch(fRxState) {
        case kIdleState:
            if (b == 0x55)
                newState = kSync1State;
            break;
        case kSync1State:
            if (b == 0xff)
                newState = kSync2State;
            else
                reset = true;
            break;
        case kSync2State:
            if (b == 0x00) {
                fRxLength = 0;
                newState = kDataState;
            }
            else
                reset = true;
            break;
        case kDataState:
            // verify that odd bytes are complements of previous bytes
            if ((fRxLength & 1) && (b + fRxData[fRxLength-1] != 0xff)) {
                // complement failed
                reset = true;
            }
            else if (fRxLength < kMaxRxData) {
                fRxData[fRxLength++] = b;
            }
            else {
                reset = true;
            }
            break;
    }

    if (reset) {
        newState = kIdleState;

        switch(b) {
            case 0x55:
                newState = kSync1State;
                break;
            case 0xff:
                if (fRxState==kDataState && fRxLength > 0
                    && fRxData[fRxLength-1]==0x55)
                        newState = kSync2State;
                break;
        }

    }

    fRxState = newState;
}


int RCX_PipeTransport::ExpectedReceiveLen(const int rxExpected)
{
    int result = rxExpected;

    if (fComplementData)
        result *= 2;

    if ((fFastMode && ((fTxLastCommand & 0xf7) == 0xa5)) || fComplementData)
        result += 2;
    else
        result += 1;

    // allow at least one byte for the header
    result += 3; // 3 byte header
    return result;
}

int RCX_PipeTransport::VerifyReply(const int rxExpected, const UByte *data, int length, UByte cmd)
{
    UByte dataSum = data[0];
    const UByte *ptr = data;
    const UByte *match = nil;
    int width = (fComplementData ? 2 : 1);
    const UByte *end = data + length + 1 - width;

    // this is a hack to work around the problem that the reply for
    // opcode a5 has a complemented command byte even in fast mode
    bool complementCmd;
    if (fFastMode && ((cmd & 0xf7) == 0xa5)) {
        complementCmd = true;
    } else {
        complementCmd = fComplementData;
    }

    // always need a cmd and a checksum
    if (length < ((complementCmd ? 2 : 1) + width)) {
        return 0;
    }

    // check the cmd
    if ((*ptr & 0xf7) != (~cmd & 0xf7)) {
        return 0;
    }
    ptr++;

    if (complementCmd) {
        if ((*ptr & 0xf7) != (cmd & 0xf7)) {
            return 0;
        }
        ptr++;
    }

    while (ptr < end) {
        if (fComplementData && ((ptr[0] & 0xf7) != (~ptr[1] & 0xf7))) {
            break;
        }

        if (ptr[0] == ComputeChecksum(dataSum, fTarget)) {
            match = ptr;
        }

        dataSum += ptr[0];
        ptr += width;
    }

    // certain spybot responses have screwed-up checksums when
    // communicating via USB tower
    if (!match && (fTarget == kRCX_SpyboticsTarget) && fPipe->IsUSB()) {
        if (length == rxExpected+1) {
            match = end - 1;
        }
    }

    if (!match) {
        return 0;
    }

    return ((match - data) / width);
}


UByte ComputeChecksum(UByte dataSum, RCX_TargetType targetType)
{
    if (targetType == kRCX_SpyboticsTarget) {
        // preamble + dataSum + checksum = 0
        return -0x98 - dataSum;
    }
    else {
        // checksum = dataSum
        return dataSum;
    }
}

int FindSync(const UByte *data, int length, const UByte *sync, const UByte cmd)
{
    int syncLen = *sync++;
    while (syncLen > 0) {
        const UByte *end = data + length - syncLen + 1;
        const UByte *ptr;

        for (ptr=data; ptr<end; ptr++) {
            int i;
            for (i=0; i<syncLen; i++) {
                if (ptr[i] != sync[i]) {
                    break;
                }
            }
            
            // check the next byte to see if it matches the command.
            // if it doesn't then we haven't really found the sync
            if ((i==syncLen) && ((ptr[syncLen] & 0xf7) == (~cmd & 0xf7))) {
                return ptr - data + syncLen;
            }
        }
        sync++;
        syncLen--;
    }

    return 0;
}
