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

/*
 * This Pipe is layered on Lego's Ghost API.  It has certain limitations
 * which make the RCX_SerialPipe preferable when serial port is being used.
 * However, for USB support on Mac OS 9, Ghost is the only viable option.
 *
 * Limitations of the GhostPipe:
 *	- fast mode is not supported (no fast firmware download)
 *	- always uses the USB Port for Ghost (no Ghost serial support)
 *	- deviceName is ignored, first USB device is always selected
 *  - user specified Rx Timeout is ignored (default Ghost timing always used)
 */

#ifndef __RCX_GhostTransport_h
#define __RCX_GhostTransport_h

#ifndef __RCX_Transport_h
#include "RCX_Transport.h"
#endif

#ifndef __GHOSTAPI_H__
#include "GhostAPI.h"
#endif

#include <vector>

using std::vector;

class RCX_GhostTransport : public RCX_Transport
{
public:
				RCX_GhostTransport();
	virtual		~RCX_GhostTransport();

	virtual RCX_Result 	Open(RCX_TargetType target, const char *deviceName, ULong options);
	virtual void		Close();

	virtual RCX_Result	Send(const UByte *txData, int txLength, UByte *rxData, int rxExpected, int rxMax, bool retry);

private:
	GHQUEUE		CreateQueue(const UByte *txData, int txLength, int rxExpected);
	RCX_Result	ExtractReply(GHQUEUE queue, UByte *rxData, int rxMax);

	GHSTACK	fStack;
	bool	fVerbose;
	bool	fOpen;
	int32	fExRetries;
	int32	fDownRetries;
};


#endif
