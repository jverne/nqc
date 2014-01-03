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
