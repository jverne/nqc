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

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cctype>

#include "RCX_Link.h"
#include "RCX_Cmd.h"
#include "rcxnub.h"
#include "rcxnub_odd.h"
#include "RCX_PipeTransport.h"
#include "RCX_SerialPipe.h"
#include "PDebug.h"
#include "RCX_Image.h"
#include "RCX_SpyboticsLinker.h"

#ifdef GHOST
#include "RCX_GhostTransport.h"
#endif

using std::printf;
using std::getenv;
using std::tolower;

#define kSerialPortEnv	"RCX_PORT"

#define kFragmentChunk	20
#define kSpyboticsSmallChunk 2
#define kSpyboticsChunk	16
#define kFirmwareChunk	200
#define kDownloadWaitTime 300

#define kNubStart 0x8000

extern bool gQuiet;

static int Checksum(const UByte *data, int length);

static bool gUSB = false;

RCX_Link::RCX_Link()
{
	fTransport = 0;
        fOmitHeader = false;
        fRCXProgramChunkSize  = kFragmentChunk;
        fRCXFirmwareChunkSize = kFirmwareChunk;
        fDownloadWaitTime     = kDownloadWaitTime;
        fVerbose = false;
}


RCX_Link::~RCX_Link()
{
	Close();
}


RCX_Result RCX_Link::Open(RCX_TargetType target, const char *portName, ULong options)
{
 	fVerbose = (options & kVerboseMode);

	fTarget = target;

	// see if an environment variable is set, otherwise use default serial device
	if (!portName) portName = getenv(kSerialPortEnv);

	const char *devName;

	if (portName && ((devName=CheckPrefix(portName, "usb")) != 0))
	{
                // USB Tower
                gUSB = true;
#ifdef GHOST
		fTransport = new RCX_GhostTransport();
#else
		RCX_Pipe *pipe = RCX_NewUSBTowerPipe();
		if (!pipe) return kRCX_USBUnsupportedError;
		fTransport = new RCX_PipeTransport(pipe);
#endif
	}
	else
	{
		// Serial Tower
                gUSB = false;
		if (portName) {
			// strip off "serial:" prefix if present
			devName = CheckPrefix(portName, "serial");
			if (!devName) devName = portName;
		}
		else {
			devName = PSerial::GetDefaultName();
		}

		fTransport = new RCX_PipeTransport(new RCX_SerialPipe());
	}

        fTransport->SetOmitHeader(fOmitHeader);

	RCX_Result result;
	result = fTransport->Open(target, devName, options);
	PREQUIRENOT(result, Fail_Open);

        if (fTarget == kRCX_SpyboticsTarget)
        {
          // turn off pinging
          RCX_Cmd cmd;
          result = Send(cmd.MakeSet(RCX_VALUE(kRCX_SpybotPingCtrlType, 1), RCX_VALUE(2, 0)));
          if (RCX_ERROR(result)) return result;
        }

	fSynced = false;
	fResult = kRCX_OK;
	return kRCX_OK;

Fail_Open:
	delete fTransport;
	fTransport = 0;
	return result;
}


void RCX_Link::Close()
{
	if (fTransport)
	{
		fTransport->Close();
		delete fTransport;
		fTransport = 0;
	}
}


RCX_Result RCX_Link::Sync()
{
	RCX_Cmd cmd;
	RCX_Result result;

	if (fSynced) return kRCX_OK;

	// always start with a ping
	result = Send(cmd.MakePing());
	// if error, try a second time
	if (result == kRCX_ReplyError)
		result = Send(&cmd);
	if (RCX_ERROR(result)) return result;

	// cybermaster requires an unlock also
	if (fTarget == kRCX_CMTarget)
	{
		result = Send(cmd.MakeUnlockCM());
		if (RCX_ERROR(result)) return result;
	}
	else if (fTarget == kRCX_ScoutTarget )
	{
		result = Send(cmd.MakeBoot());
		if (RCX_ERROR(result)) return result;

		result = Send(cmd.Set(0x47, 0x80));
		if (RCX_ERROR(result)) return result;
	}


	fSynced = true;
	return kRCX_OK;
}


bool RCX_Link::WasErrorFromMissingFirmware()
{
	// if not RCX, RCX2, or Swan then firmware isn't required
	if ((fTarget != kRCX_RCXTarget) &&
            (fTarget != kRCX_RCX2Target) &&
            (fTarget != kRCX_SwanTarget)) return false;

	// if not synced, then firmware wasn't a problem
	if (!fSynced) return false;

	// use Unlock command to check ROM/firmware versions
	RCX_Result result;
	RCX_Cmd cmd;
	result = Send(cmd.MakeUnlock());


	// check the reply
	if (result != 8) return false;
	for(int i=4; i<8; ++i) {
		if (GetReplyByte(i) != 0) return false;
	}

	return true;
}


RCX_Result RCX_Link::Download(const RCX_Image &image, int programNumber)
{
	RCX_Result result;
	RCX_Cmd cmd;

	// sync with RCX
	result = Sync();
	if (RCX_ERROR(result)) return result;

	// stop any running tasks
	result = Send(cmd.Set(kRCX_StopAllOp));
	if (RCX_ERROR(result)) return result;

	if (fTarget == kRCX_SpyboticsTarget)
	{
		result = DownloadSpybotics(image);
	}
	else
	{
		result = DownloadByChunk(image, programNumber);
	}

	if (RCX_ERROR(result)) return result;

        if (!gQuiet) // play sound when done
	    Send(cmd.MakePlaySound(5));
	return kRCX_OK;
}


RCX_Result RCX_Link::DownloadSpybotics(const RCX_Image &image)
{
	RCX_SpyboticsLinker linker;
	vector<UByte> output;
	RCX_Result result;
	RCX_Cmd cmd;

	linker.Generate(image, output);
	int length = output.size();
	const UByte* data = &output[0];

	int check = Checksum(data, length);
	int start = 0x100;
	result = Send(cmd.Set(kRCX_BeginFirmwareOp, (UByte)(start), (UByte)(start>>8),
		(UByte)check, (UByte)(check>>8), 0));
	if (RCX_ERROR(result)) return result;

	BeginProgress(length);
	result = Download(data, length, gUSB ? kSpyboticsSmallChunk : kSpyboticsChunk);
	if (RCX_ERROR(result)) return result;

	return kRCX_OK;
}

bool gProgramMode = false;
class ProgramMode
{
  public:
  ProgramMode() { gProgramMode = true; }
  ~ProgramMode() { gProgramMode = false; }
};

RCX_Result RCX_Link::DownloadByChunk(const RCX_Image &image, int programNumber)
{
        ProgramMode x = ProgramMode();

	RCX_Result result;
	RCX_Cmd	cmd;
	int i;

	// select program
	if (programNumber)
	{
		result = Send(cmd.Set(kRCX_SelectProgramOp, (UByte)(programNumber-1)));
		if (result < 0) return result;
	}

	// clear existing tasks and/or subs
	result = Send(cmd.MakeDeleteTasks());
	if (RCX_ERROR(result)) return result;

	result = Send(cmd.MakeDeleteSubs());
	if (RCX_ERROR(result)) return result;

	int total = image.GetSize();

	for(i=0; i<image.GetChunkCount(); i++)
	{
		const RCX_Image::Chunk &f = image.GetChunk(i);
		result = DownloadChunk(f.GetType(), f.GetNumber(), f.GetData(), f.GetLength(), i==0 ? total : -1);
		if (RCX_ERROR(result)) return result;
	}

	return kRCX_OK;
}


RCX_Result RCX_Link::DownloadChunk(RCX_ChunkType type, UByte number, const UByte *data, int length, int total)
{
	RCX_Cmd cmd;
	RCX_Result result;

	result = Sync();
	if (RCX_ERROR(result)) return result;

	result = Send(cmd.MakeBegin(type, number, (UShort)length));
	if (RCX_ERROR(result)) return result;

	// make sure we have enough room
	if (result != 1 ||
		GetReplyByte(0) != 0) return kRCX_MemFullError;

	if (total == 0)
		total = length;

	if (total > 0)
		BeginProgress(total);

	result = Download(data, length, fRCXProgramChunkSize);
	return result;
}


RCX_Result RCX_Link::GetVersion(ULong &rom, ULong &ram)
{
	RCX_Cmd cmd;
	RCX_Result result;
	UByte reply[8];

	result = Sync();
	if (RCX_ERROR(result)) return result;

	result = Send(cmd.MakeUnlock());
	if (RCX_ERROR(result)) return result;

	if (result != 8) return kRCX_ReplyError;

	GetReply(reply,8);

	rom =	((ULong)reply[0] << 24) |
			((ULong)reply[1] << 16) |
			((ULong)reply[2] << 8) |
			((ULong)reply[3]);

	ram =	((ULong)reply[4] << 24) |
			((ULong)reply[5] << 16) |
			((ULong)reply[6] << 8) |
			((ULong)reply[7]);

	return kRCX_OK;
}


RCX_Result RCX_Link::GetValue(RCX_Value value)
{
	RCX_Cmd cmd;
	RCX_Result result;

	result = Sync();
	if (RCX_ERROR(result)) return result;

	result = Send(cmd.MakeRead(value));
	if (RCX_ERROR(result)) return result;
	if (result != 2) return kRCX_ReplyError;

	result = (int)GetReplyByte(0) + ((int)GetReplyByte(1) << 8);
	return result;
}


RCX_Result RCX_Link::GetBatteryLevel()
{
	RCX_Cmd cmd;
	RCX_Result result;

	result = Sync();
	if (RCX_ERROR(result)) return result;

	if (fTarget == kRCX_ScoutTarget)
	{
		result = Send(cmd.Set(kRCX_PollMemoryOp, 0x3a, 0x01, 0x01));
		if (result != 1) return kRCX_ReplyError;
		result = (int)GetReplyByte(0) * 109;
	}
	else
	{
		result = Send(cmd.Set(kRCX_BatteryLevelOp));
		if (result != 2) return kRCX_ReplyError;
		result = (int)GetReplyByte(0) + ((int)GetReplyByte(1) << 8);
	}

	return result;
}


RCX_Result RCX_Link::DownloadFirmware(const UByte *data, int length, int start, bool fast)
{
	RCX_Result result;

	if (fast)
	{
		// check for fast mode support
		if (!fTransport->FastModeSupported()) return kRCX_PipeModeError;

		// send the nub first
		if (fTransport->FastModeOddParity()) {
			result = TransferFirmware(rcxnub_odd, sizeof(rcxnub_odd), kNubStart, false);
		}
		else {
			result = TransferFirmware(rcxnub, sizeof(rcxnub), kNubStart, false);
		}
		if (RCX_ERROR(result)) return result;

		// switch to fast mode
		fTransport->SetFastMode(true);

		// download
		result = TransferFirmware(data, length, start, true);

		fTransport->SetFastMode(false);
	}
	else
	{
		result = TransferFirmware(data, length, start, true);
	}

	return result;
}


RCX_Result RCX_Link::TransferFirmware(const UByte *data, int length, int start, bool progress)
{
	RCX_Cmd cmd;
	RCX_Result result;

	result = Sync();
	if (RCX_ERROR(result)) return result;

	result = Send(cmd.MakeUnlock());
	if (RCX_ERROR(result)) return result;

	result = Send(cmd.Set(kRCX_BootModeOp, 1, 3, 5, 7, 0xb));
	if (RCX_ERROR(result)) return result;

	int check = Checksum(data, length < 0x4c00 ? length : 0x4c00);
	result = Send(cmd.Set(kRCX_BeginFirmwareOp, (UByte)(start), (UByte)(start>>8),
		(UByte)check, (UByte)(check>>8), 0));
	if (RCX_ERROR(result)) return result;

	BeginProgress(progress ? length : 0);
	result = Download(data, length, fRCXFirmwareChunkSize);
	if (RCX_ERROR(result)) return result;

	// last packet is no-retry with an extra long delay
	// this gives the RCX time to respond and makes sure response doesn't get trampled
	result = Send(cmd.MakeBoot(), false);
        if (fTransport->GetFastMode())
          return kRCX_OK;

	return result;
}

const int nOnesDensity[256] = {
  0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
  };

#define max(x, y) ((x) > (y)) ? (x) : (y)

int RCX_Link::AdjustChunkSize(const int n, const int nMaxZeros, const int nMaxOnes,
  const UByte *data, bool bComplement)
{
  int size = n;
  //
  // Avoid long strings of zeroes -- fast downloading doesn't like it and messaging
  // can loose sync. Especially with short distances and transmitter set to long
  // range [i.e. high power]
  //
  // Only need this check when complement byte transmission is disabled
  //
  if (!bComplement)
  {
    const int kOnesPlusMinusScore = 3;

    int i;
    for (i = 0; i < (size - nMaxZeros); ++i)
    {
      int j;

      UByte * p = (UByte*)data + i;
      if (*p != 0)
        continue;
      //
      // found a zero -- check to see how many
      //
      for (j = 0; j < nMaxZeros; ++j)
      {
        UByte * q = (UByte*)data + i + j;
        if (*q != 0)
          break;
      }
      if (j >= nMaxZeros)
      {
        // too many consecutive zeros. Shorten the message size
        //
        size = i + nMaxZeros;
        if (fVerbose)
                printf("too many consecutive zeros (%d)\n", j);
        break;
      }
    }

    for (i = 0; i < (size - nMaxOnes); ++i)
    {
      int j;
      ubyte aByte;
      int nLotsOfOnes;

      aByte = *(data + i);
      if (nOnesDensity[aByte] >= 3)
        continue;
      //
      // found a sparse byte -- check to see how many consecutive
      //
      nLotsOfOnes = 0;
      for (j = 0; j < nMaxOnes; ++j)
      {
        aByte = *(data + i + j);
        if (nOnesDensity[aByte] >= 3)
        {
          ++nLotsOfOnes;
          if (nLotsOfOnes > kOnesPlusMinusScore)
            break;
        }
        else
        {
              nLotsOfOnes -= 2;
              nLotsOfOnes = max(0, nLotsOfOnes);
        }
      }
      if (j >= nMaxOnes)
      {
        // too many consecutive sparse bytes. Shorten the message size
        //
        size = max(i, nMaxOnes);
        if (fVerbose)
                printf("too many consecutive sparse bytes (%d)\n", j);
        break;
      }
    }

  }
  return size;
}

RCX_Result RCX_Link::Download(const UByte *data, int length, int chunk)
{
	RCX_Cmd cmd;
	RCX_Result result;
	UShort seq;
	int remain = length;
	int n;

	seq = 1;
	while(remain > 0)
	{
		if (remain <= chunk)
		{
                        if (!gQuiet || gProgramMode)
			  seq = 0;
			n = remain;
		}
		else
			n = chunk;

                int maxZeros = gUSB ? 23 : 30;
                int maxOnes  = 90;
                n = AdjustChunkSize(n, maxZeros, maxOnes, data, fTransport->GetComplementData());
                if (fVerbose)
                        printf("sending %d bytes\n", n);

		result = Send(cmd.MakeDownload(seq++, data, (UShort)n), true, fDownloadWaitTime);
		if (result < 0) return result;

		remain -= n;
		data += n;
		if (!IncrementProgress(n)) return kRCX_AbortError;
	}

	return kRCX_OK;
}


RCX_Result RCX_Link::Send(const RCX_Cmd *cmd, bool retry, int timeout)
{
	return Send(cmd->GetBody(), cmd->GetLength(), retry, timeout);
}


RCX_Result RCX_Link::Send(const UByte *data, int length, bool retry, int timeout)
{
	int expected = ExpectedReplyLength(data, length);

	if (length > kMaxCmdLength ||
		expected > kMaxReplyLength) return kRCX_RequestError;

	fResult = fTransport->Send(data, length, fReply, expected, kMaxReplyLength, retry, timeout);

	return fResult;
}



RCX_Result RCX_Link::GetReply(UByte *data, int maxLength)
{
	if (fResult < 0) return fResult;

	int length = fResult;
	if (length > maxLength) length = maxLength;

	const UByte *src = fReply+1;

	for(int i=0; i<length; ++i)
	{
		*data++ = *src++;
	}

	return length;
}


void RCX_Link::BeginProgress(int total)
{
	fDownloadTotal = total;
	fDownloadSoFar = 0;
}


bool RCX_Link::IncrementProgress(int delta)
{
	fDownloadSoFar += delta;
	return fDownloadTotal ? DownloadProgress(fDownloadSoFar, fDownloadTotal) : true;
}


bool RCX_Link::DownloadProgress(int /* soFar */, int /* total */)
{
	return true;
}


int RCX_Link::ExpectedReplyLength(const UByte *data, int length)
{
	switch(data[0] & 0xf7)
	{
/*
		case 0xf7:	// message
		case 0xd2:	// remote
			return 0;	// unconfirmed
*/
		case 0x25:	// __task
		case 0x35:
		case 0x45:	// __dl
		case 0x75:
			return 2;
		case 0x30:	// pollb
		case 0x12:	// poll
			return 3;
		case 0x15:
			return 9;
                case 0x11: // spybot upload EEPROM
                        return (fTarget == kRCX_CMTarget) ? 1 : 17;
		case 0xa5:
			return 26;
		case 0x20:
			return (fTarget == kRCX_CMTarget) ? 21 : 189;
		case 0x63:	// pollm
			if (length != 4) return 0;
			return data[3] +1;
		case 0xa4:	// upload datalog
			if (length != 5) return 0;
			return (data[3] + ((int)data[4] << 8)) * 3 + 1;
		default:
			return 1;
	}
}


const char *CheckPrefix(const char *s, const char *prefix)
{
	while(*prefix)
	{
		if (tolower(*prefix) != tolower(*s)) return 0;
		prefix++;
		s++;
	}

	// must end with ':' or 0, absorb ':'
	switch(*s)
	{
		case ':':
			return s+1;
		case 0:
			return s;
		default:
			return 0;
	}
}



int Checksum(const UByte *data, int length)
{
	int check = 0;
	while(length--) {
		check += *data++;
	}
	return check;
}


