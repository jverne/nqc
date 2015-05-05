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
#ifndef __PSerial_h
#define __PSerial_h

#ifndef __PTypes_h
#include "PTypes.h"
#endif

#ifndef __PStream_h
#include "PStream.h"
#endif


// serial port settings
#define kPSerial_DataMask       0x03
#define kPSerial_DataShift      0
#define kPSerial_Data8          (0 << kPSerial_DataShift)
#define kPSerial_Data7          (1 << kPSerial_DataShift)
#define kPSerial_Data6          (2 << kPSerial_DataShift)
#define kPSerial_Data5          (3 << kPSerial_DataShift)

#define kPSerial_ParityMask     0x0c
#define kPSerial_ParityShift    2
#define kPSerial_ParityNone     (0 << kPSerial_ParityShift)
#define kPSerial_ParityOdd      (1 << kPSerial_ParityShift)
#define kPSerial_ParityEven     (2 << kPSerial_ParityShift)

#define kPSerial_StopMask       0x30
#define kPSerial_StopShift      4
#define kPSerial_Stop1          (0 << kPSerial_StopShift)
#define kPSerial_Stop1_5        (1 << kPSerial_StopShift)
#define kPSerial_Stop2          (2 << kPSerial_StopShift)


class PSerial : public PStream
{
public:
    virtual bool    Open(const char *name) = 0;

    virtual bool    SetSpeed(int speed, int opts = 0) = 0;
    virtual bool    SetDTR(bool /* assert */) { return false; }
    virtual bool    SetRTS(bool /* assert */) { return false; }
    void SetIsSpy(bool value) { fSpy = value; }
    bool GetIsSpy(void) { return fSpy; }

    static PSerial*     NewSerial();
    static const char*  GetDefaultName();

protected:
    bool fSpy;
};


#endif
