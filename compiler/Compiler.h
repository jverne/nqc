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
#ifndef __Compiler_h
#define __Compiler_h

#include <vector>

#include "RCX_Constants.h"
#include "RCX_Target.h"
#include "RCX_Disasm.h"

using std::vector;

class RCX_Image;
class Buffer;

class Compiler : public RCX_SourceFiles
{
public:
	enum
	{
		kCompat_Flag = 1 << 0,
		kNoSysFile_Flag = 1 << 2
	};

			Compiler()	 { sCompiler = this; fDirty = false; }
	virtual ~Compiler() {}

	static Compiler*	Get()	{ return sCompiler; }

	void	Reset();
	RCX_Image *	Compile(Buffer *buffer, const RCX_Target *target, int flags);

	void	Define(const char *name, const char *value=0);
	void	Undefine(const char *name);

	// hooks for the lexer
	int				AddBuffer(Buffer *b);
	Buffer*			GetBuffer(int i)		{ return fBuffers[i]; }

	virtual Buffer *CreateBuffer(const char *name) = 0;

	// access to system API buffer
	static	Buffer*		CreateApiBuffer(bool compatMode);

	// used to insert source code into assembly listings (from RCX_SourceFiles)
	virtual long Print(RCX_Printer *printer, short index, long start, long end);

private:
	void	ReleaseBuffers();

	vector<Buffer*>	fBuffers;
	bool			fDirty;

	static Compiler*	sCompiler;
};

#endif
