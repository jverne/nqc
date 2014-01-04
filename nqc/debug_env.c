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
/*
 * This file is strictly for the Macintosh debug build and can be used
 * to test the effects of different environment variable settings by
 * adding the appropriate name/value pair to sEnvironment.  Most release
 * builds of NQC rely on a real version of getenv() that actually reads
 * environment variables.
 */

#include <cstdlib>
#include <cstring>

#if defined(__cplusplus) && defined(_MSL_USING_NAMESPACE)
namespace std {
#endif

static char *sEnvironment[]=
{
//	"NQC_INCLUDE", "::nqc_src:",

//	"NQC_OPTIONS", "-Trcx2 -v -raw 120e00",
	"NQC_OPTIONS", "-Susb -Tspy -l foo.nqc",
//	"NQC_OPTIONS", "-firmfast firm0330.lgo",
//	"RCX_PORT", "/dev/cu.USA19QI21P1.1",
//	"RCX_PORT", "/dev/cu.IrDA-IrCOMMch-b",
	"RCX_PORT", "usb",
};


#define kEnvCount	(sizeof(sEnvironment) / (2 * sizeof(const char *)))

// hack to disable SIOUX delays
#ifdef SIOUX
extern "C" Boolean		SIOUXUseWaitNextEvent;	/*	Can we use WaitNextEvent? ...*/
#endif

char* getenv(const char *name)
{
#ifdef SIOUX
	static bool inited = false;
	if (!inited) {
		printf("\n");
		SIOUXUseWaitNextEvent = false;
		inited = true;
	}
#endif
	int i;

	for(i=0; i<kEnvCount; i++)
		if (strcmp(sEnvironment[2*i], name)==0)
			return sEnvironment[2*i+1];

	return 0;
}



#if defined(__cplusplus) && defined(_MSL_USING_NAMESPACE)
}
#endif
