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
 * Portions created by David Baum are Copyright (C) 2000 David Baum.
 * All Rights Reserved.
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
