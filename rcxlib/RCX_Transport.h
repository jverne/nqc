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
#ifndef __RCX_Transport_h
#define __RCX_Transport_h

#ifndef __PTypes_h
#include "PTypes.h"
#endif

#ifndef __RCX_Result_h
#include "RCX_Result.h"
#endif

#ifndef __RCX_Constants_h
#include "RCX_Constants.h"
#endif

#ifndef __RCX_Target_h
#include "RCX_Target.h"
#endif


class RCX_Transport
{
public:
			RCX_Transport() {}
	virtual		~RCX_Transport() {};

	virtual RCX_Result 	Open(RCX_TargetType target, const char *deviceName, ULong options) = 0;
	virtual void		Close() = 0;

	virtual RCX_Result	Send(const UByte *txData, int txLength, UByte *rxData, int rxExpected, int rxMax, bool retry, int timeout) = 0;

	virtual bool		FastModeSupported() const { return false; }
	virtual bool		FastModeOddParity() const { return false; }
	virtual void		SetFastMode(bool /* fast */) { }
        virtual bool            GetFastMode() const { return false; }
        void                    SetOmitHeader(bool value) { fOmitHeader = value; }
        virtual bool            GetComplementData() const { return false; }

protected:
        static void DumpData(const UByte *ptr, int length);
        bool fOmitHeader;

private:
};


#endif
