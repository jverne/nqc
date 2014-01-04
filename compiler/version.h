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
#ifndef __version_h
#define __version_h

#define MAJOR_VERSION	3
#define MINOR_VERSION	1
#define RELEASE_STAGE	"r"

// 0=alpha, 2=release
#define RELEASE_STAGE_INDEX 2

#define RELEASE_BUILD	6

#define STR_(x)	#x
#define STR(x)	STR_(x)

#define VERSION_STRING STR(MAJOR_VERSION) "." STR(MINOR_VERSION) " " RELEASE_STAGE STR(RELEASE_BUILD) "\0"

#endif
