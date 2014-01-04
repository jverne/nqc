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
#ifndef __RCX_Result_h
#define __RCX_Result_h

typedef int RCX_Result;

// result codes
#define kRCX_OK					0
#define kRCX_OpenSerialError	-1	// serial port could not be opened and/or configured
#define kRCX_IREchoError		-2	// no echo receieved from IR tower
#define kRCX_ReplyError			-3	// no (or invalid) reply from RCX
#define kRCX_RequestError		-4	// attempt to send too much data
#define kRCX_FileError			-5	// could not open/read/write file
#define kRCX_FormatError		-6	// unknown file format
#define kRCX_AbortError			-7	// canceled by RCX_Link::DownloadProgress()
#define kRCX_MemFullError		-8	// not enough room in RCX program memory

// new error codes as of 2.4a3
#define kRCX_PipeModeError		-9	// feature not supported by the pipe (e.g. CM target on IR tower)
#define kRCX_USBUnsupportedError	-10	// USB is not yet supported (depends on the OS)
#define kRCX_GhostNotFoundError		-11	// USB requires Ghost, which is not installed

#define kRCX_LastError			-11	//	last error code

#define RCX_ERROR(result)	((result) < 0)

#endif
