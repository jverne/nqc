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
 /*
  * 08/11/01    dfb Lots of changes to accomodate Ghost API
  *         - Send1() has been removed (used retry=false in Send() instead)
  *         - Target, RxTimeout, and Verbose are set as part of Open() rather than
  *             having their own calls.  This simplifies things since those properties
  *             are then invariant once the link has been opened.
  *         - It is no longer possible to specify a timeout in Send() calls
  */

#ifndef __RCX_Link_h
#define __RCX_Link_h

#ifndef __RCX_Result_h
#include "RCX_Result.h"
#endif

#ifndef __RCX_Constants_h
#include "RCX_Constants.h"
#endif

#ifndef __RCX_Target_h
#include "RCX_Target.h"
#endif

#ifndef __RCX_Transport_h
#include "RCX_Transport.h"
#endif

class RCX_Cmd;
class RCX_Image;

class RCX_Link
{
public:
    enum {
        // constants for Open options
        kRxTimeoutMask = 0x0000ffff,
        kVerboseMode = 0x80000000,

        kMaxCmdLength = 4000,   ///< Maximum command size (much bigger than strictly necessary)
        kMaxReplyLength = 1000  ///< Maximum reply size (much bigger than strictly necessary)
    };

    RCX_Link();
    virtual ~RCX_Link();

    RCX_Result Open(RCX_TargetType target, const char *portName = nil,
        ULong options=0);
    void Close();

    RCX_Result Sync();

    RCX_Result Send(const RCX_Cmd *cmd, bool retry=true, int timeout=0);
    RCX_Result Send(const UByte *data, int length, bool retry=true,
        int timeout=0);

    /**
     * Only looks at the reply data - the
     * inverted command byte is ignored.  This makes the indexing consistent
     * with RCX_Result (which represents the number of data bytes in the reply)
     */
    RCX_Result GetReply(UByte *data, int maxLength);

    /**
     * Only looks at the reply data - the
     * inverted command byte is ignored.  This makes the indexing consistent
     * with RCX_Result (which represents the number of data bytes in the reply)
     */
    UByte GetReplyByte(int index) const { return fReply[index + 1]; }

    RCX_Result Download(const RCX_Image &image, int programNumber);
    RCX_Result DownloadFirmware(const UByte *data, int length, int start,
        bool fast);

    virtual bool DownloadProgress(int soFar, int total, int chunkSize);

    RCX_Result GetVersion(ULong &rom, ULong &ram);
    RCX_Result GetBatteryLevel();
    RCX_Result GetValue(RCX_Value value);

    /// This function should only be called after Sync() has been called.
    /// It can be used to determine if an kRCX_ReplyError error was due
    /// to missing firmware.
    bool WasErrorFromMissingFirmware();

    void SetOmitHeader(bool value) {
        fOmitHeader = value;
        if (fTransport) fTransport->SetOmitHeader(value);
    }
    void SetRCXProgramChunkSize(int value) {
        fRCXProgramChunkSize = value;
    }
    void SetRCXFirmwareChunkSize(int value) {
        fRCXFirmwareChunkSize = value;
    }
    void SetDownloadWaitTime(int value) {
        fDownloadWaitTime = value;
    }

private:
    RCX_Result DownloadByChunk(const RCX_Image &image, int programNumber);
    RCX_Result DownloadSpybotics(const RCX_Image &image);
    RCX_Result DownloadChunk(RCX_ChunkType type, UByte taskNumber,
        const UByte *data, int length, int total=0);
    RCX_Result  TransferFirmware(const UByte *data, int length, int start,
        bool progress);
    RCX_Result  Download(const UByte *data, int length, int chunk);

    int ExpectedReplyLength(const UByte *data, int length);
    int AdjustChunkSize(const int n, const int nMaxZeros,
        const int nMaxOnes, const UByte *data, bool bComplement);
    void BeginProgress(int total);
    bool IncrementProgress(int delta);

    RCX_Transport* fTransport;
    bool fSynced;
    RCX_TargetType fTarget;
    bool fOmitHeader;
    int fRCXProgramChunkSize;
    int fRCXFirmwareChunkSize;
    int fDownloadWaitTime;
    bool fVerbose;

    RCX_Result fResult;
    UByte fReply[kMaxReplyLength];  // includes command and data

    // new fields to track download progress;
    int fDownloadTotal;
    int fDownloadSoFar;
};

const char *CheckPrefix(const char *s, const char *prefix);

#endif
