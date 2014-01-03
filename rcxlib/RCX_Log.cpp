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


#include <cstdio>
#include <cstring>
#include "RCX_Log.h"
#include "RCX_Cmd.h"
#include "RCX_Link.h"

using std::sprintf;
using std::strlen;

#define kPointsPerUpload	10

RCX_Log::RCX_Log()
{
	fLength = 0;
	fData = nil;
	fTypes = nil;
}


RCX_Log::~RCX_Log()
{
	SetLength(0);
}



void RCX_Log::SetLength(int length)
{
	// clear old data
	if (fLength)
	{
		delete [] fData;
		delete [] fTypes;
	}

	if (length)
	{
		fData = new short[length];
		fTypes = new UByte[length];
	}
	else
	{
		fData = nil;
		fTypes = nil;
	}

	fLength = length;
}


RCX_Result RCX_Log::Upload(RCX_Link *link)
{
	RCX_Result result;
	RCX_Cmd cmd;
	int length;
	int i;
	int pos, n;

	link->Sync();

	// get length of log
	result = link->Send(cmd.MakeUploadDatalog(0, 1));
	if (result < 0) return result;
	if (result != 3) return kRCX_ReplyError;

	// figure out length of data (don't include the length entry itself)
	length = (link->GetReplyByte(1) + (link->GetReplyByte(2) << 8)) - 1;
	SetLength(length);

	for(pos = 0; pos<length; )
	{
		// how many points to upload
		n = length - pos;
		if (n > kPointsPerUpload)
			n = kPointsPerUpload;

		// upload the data itself
		result = link->Send(cmd.MakeUploadDatalog(pos+1, n));
		if (result < 0) return result;
		if (result != n * 3) return kRCX_ReplyError;

		// copy data into log
		for(i=0; i<n; i++)
		{
			fTypes[pos+i] = link->GetReplyByte(i*3);
			fData[pos+i] = (short)(link->GetReplyByte(i*3+1) +
							((UShort)link->GetReplyByte(i*3+2) << 8));
		}

		pos += n;
		if (!link->DownloadProgress(pos, length)) break;
	}

	return pos;
}


void RCX_Log::SPrintEntry(char *buf, int index, bool verbose) const
{
	if (verbose)
	{
		UByte type = GetType(index);
		switch(type & 0xe0)
		{
			case 0:
				sprintf(buf, "Variable %d: ", type & 0x1f);
				break;
			case 0x20:
				sprintf(buf, "Timer %d: ", type & 0x1f);
				break;
			case 0x40:
				sprintf(buf, "Sensor %d: ", (type & 0x1f) + 1);
				break;
			case 0x80:
				sprintf(buf, "Watch: ");
				break;
			default:
				sprintf(buf, "%02x: ", type);
				break;
		}
		buf += strlen(buf);
	}
	sprintf(buf, "%d", GetData(index));
}
