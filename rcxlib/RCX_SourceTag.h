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
#ifndef __RCX_SourceTag_h
#define __RCX_SourceTag_h

// a single tag that relates bytecode offset to source file/offset
class RCX_SourceTag
{
public:
	// types of source tags
	enum {
		kBegin,		// start of fragment source
		kBeginNoList,	// begin a no-list fragment
		kNormal,	// most tags are this type
		kEnd		// end of fragment source
	};

	int		fType;
	short	fAddress;
	short	fSrcIndex;
	long	fSrcOffset;
};


#endif
