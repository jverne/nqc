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

#ifndef __PDebug_h
#define __PDebug_h


#ifdef PDEBUG
#ifdef __cplusplus
extern "C" {
#endif
#if TARGET_API_MAC_OS8==1
	#define _p_debugstr(msg)	debugstr(msg)
#else
	extern void	_p_debugstr(char *text);
#endif	// macintosh

	// normal debugging
	extern void _p_require(const char* file, int line, const char* label);
	extern void _p_requirenot(const char* file, int line, int val, const char* label);
	extern void _p_assert(const char *file, int line);
#ifdef __cplusplus
}
#endif

	#define PASSERT(expr)				do { if (!(expr)) _p_assert(__FILE__, __LINE__); } while(0)
	#define PDEBUGSTR(mesg)				_p_debugstr(mesg)
	#define PREQUIRE(expr, label)		do { if (!(expr)) { _p_require(__FILE__, __LINE__, #label); goto label; }} while (0)
	#define PREQUIRENOT(expr, label)	do { if (expr) { _p_requirenot(__FILE__, __LINE__, (int)(expr), #label); goto label; }} while (0)

#else
	// no debugging

	#define PASSERT(expr)				((void)0)
	#define PDEBUGSTR(mesg)				((void)0)
	#define PREQUIRE(expr, label)		do { if (!(expr)) goto label; } while (0)
	#define PREQUIRENOT(expr, label)	do { if (expr) goto label; } while (0)

#endif	// PDEBUG


#endif	// __PDebug_h
