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
