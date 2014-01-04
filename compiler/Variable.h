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

#ifndef __Variable_h
#define __Variable_h

enum Variable {
	// error values
	kIllegalVar = -1,
	kPhysicalVarBase = 0,
	kVirtualVarBase = 	 0x8000,
	kVirtualConstantFlag = 0x4000,	// for vars that are always constant
        kVirtualReadOnlyFlag = 0x2000,	// for vars that are not lvalues
        kPointerFlag = 0x1000,	// for pointers
	kVirtualVarMask = 	 0x0fff
};


#endif
