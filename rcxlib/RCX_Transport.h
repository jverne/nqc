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
