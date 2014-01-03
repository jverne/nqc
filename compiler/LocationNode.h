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

#ifndef __LocationNode_h
#define __LocationNode_h

#ifndef __LexLocation_h
#include "LexLocation.h"
#endif

#ifndef __AutoFree_h
#include "AutoFree.h"
#endif

extern void LexCurrentLocation(LexLocation &loc);

class LocationNode : public AutoFree
{
public:
						LocationNode(const LexLocation &loc) : fLocation(loc) {}
						LocationNode() { LexCurrentLocation(fLocation); }

	const LexLocation&	GetLoc() const	{ return fLocation; }

private:
	LexLocation	fLocation;
};


#endif
