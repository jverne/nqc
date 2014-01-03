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
#include <cstdio>
#include "RCX_Transport.h"
#include "RCX_Target.h"

using std::printf;
using std::putchar;

void RCX_Transport::DumpData(const UByte *ptr, int length)
{
	int i;

	for(i=0; i<length; i++)
	{
		printf("%02x ", *ptr++);
//		if ((i%16)==15) putchar('\n');
	}

	putchar('\n');
}
