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
#ifndef __PTypes_h
#define __PTypes_h

typedef unsigned long ULong;
typedef unsigned short UShort;
typedef unsigned char UByte;

#ifndef nil
#define nil 0
#endif

#ifdef PNEED_BOOL
typedef char bool;

#ifndef false
#define false ((bool)0)
#endif

#ifndef true
#define true ((bool)1)
#endif
#endif


#endif
