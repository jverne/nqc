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
#ifndef __PDebug_h
#define __PDebug_h

#define OUTFILE stderr
#define NCQ_LOGFILE "NQC_LOGFILE"

#ifdef PDEBUG
#ifdef __cplusplus
extern "C" {
#endif
#if TARGET_API_MAC_OS8==1
	#define _p_debugstr(msg)	debugstr(msg)
#else
	extern void	_p_debugstr(const char *text);
    extern void _p_debugvar(const char* varname, const int);
#endif	// macintosh

	// normal debugging
	extern void _p_require(const char* file, int line, const char* label);
	extern void _p_requirenot(const char* file, int line, int val, const char* label);
	extern void _p_assert(const char *file, int line);
#ifdef __cplusplus
}
#endif
    // debugging

	#define PASSERT(expr)				do { if (!(expr)) _p_assert(__FILE__, __LINE__); } while(0)
	#define PDEBUGSTR(mesg)				_p_debugstr(mesg)
    #define PDEBUGVAR(varname, var)     _p_debugvar(varname, var)
	#define PREQUIRE(expr, label)		do { if (!(expr)) { _p_require(__FILE__, __LINE__, #label); goto label; }} while (0)
	#define PREQUIRENOT(expr, label)	do { if (expr) { _p_requirenot(__FILE__, __LINE__, (int)(expr), #label); goto label; }} while (0)

#else
	// no debugging

	#define PASSERT(expr)				((void)0)
	#define PDEBUGSTR(mesg)				((void)0)
    #define PDEBUGVAR(varname, var)     ((void)0)
	#define PREQUIRE(expr, label)		do { if (!(expr)) goto label; } while (0)
	#define PREQUIRENOT(expr, label)	do { if (expr) goto label; } while (0)

#endif	// PDEBUG


#endif	// __PDebug_h
