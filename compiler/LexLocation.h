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
#ifndef __LexLocation_h
#define __LexLocation_h

class Buffer;

#define kIllegalSrcIndex -1

typedef struct LexLocation
{
	short	fIndex;
	short	fLength;
	long	fOffset;
} LexLocation;

#endif

