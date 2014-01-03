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
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */

#include <cstring>
#include "Compiler.h"
#include "PreProc.h"
#include "Symbol.h"
#include "Program.h"
#include "Buffer.h"
#include "Macro.h"
#include "rcx1_nqh.h"
#include "rcx2_nqh.h"
#include "Error.h"
#include "RCX_Image.h"
#include "RCX_Target.h"
#include "version.h"

using std::sprintf;

//#define NO_AUTO_FREE


Compiler* Compiler::sCompiler = 0;


void Compiler::Reset()
{
	// this function resets the state of the compiler, freeing
	// up any memory used by it.  Beware, any references to
	// program fragments, statements, conditions, or symbols
	// will become invalid after this

	LexReset();

	delete gPreProc;
	delete gProgram;
	gProgram = 0;
	Symbol::GetSymbolTable()->DeleteAll();

#ifndef NO_AUTO_FREE
	GetAutoFreeGroup().freeAll();
#endif

	gPreProc = new PreProc();

	ReleaseBuffers();

	fDirty = false;
}


int Compiler::AddBuffer(Buffer *b)
{
	int index = fBuffers.size();
	fBuffers.push_back(b);
	return index;
}


RCX_Image *Compiler::Compile(Buffer *b, const RCX_Target *target, int flags)
{
	// reset compiler if needed
	if (fDirty)
	{
		Reset();
	}

	fDirty = true;
	gProgram = new Program(target);

	// define compiler target and compat mode
	Define(target->fDefine, target->fDefValue);
	if (flags & kCompat_Flag)
		Define("__NQC1");

	// define NQC version number
	char versionCode[5];
	sprintf(versionCode, "%d", MAJOR_VERSION * 100 + MINOR_VERSION);
	Define("__NQC__", versionCode);

	LexPush(b);

	// system file
	if ((flags & kNoSysFile_Flag) == 0)
	{
		LexPush(CreateApiBuffer(flags & kCompat_Flag));
	}

	ErrorHandler::Get()->Reset();

	yyparse();

	RCX_Image *image = 0;
	if (ErrorHandler::Get()->GetErrorCount() == 0)
	{
		image = gProgram->CreateImage();
		if (ErrorHandler::Get()->GetErrorCount())
		{
			delete image;
			image = 0;
		}
	}

	return image;
}


Buffer *Compiler::CreateApiBuffer(bool compatMode)
{
	Buffer *b = new Buffer();

	if (compatMode)
		b->Create("rcx.nqh", rcx1_nqh, sizeof(rcx1_nqh));
	else
		b->Create("rcx.nqh", rcx2_nqh, sizeof(rcx2_nqh));

	return b;
}


void Compiler::Define(const char *name, const char *value)
{
	Symbol *s = Symbol::Get(name);
	Macro *m;

	if (value)
	{
		vector<Token> tokens;
		Buffer *buf;
		int t;
		TokenVal v;
		int count = 0;

		buf = new Buffer();

		buf->Create("<cmdline>", value, (int)strlen(value));
		LexPush(buf);

		while((t=yylex(v)) != 0)
		{
			tokens.resize(count+1);
			tokens[count].fType = t;
			tokens[count].fValue = v;
			count++;
		}
		m = new Macro(&tokens[0], tokens.size(), Macro::kNoArgs);
	}
	else
		m = new Macro(0, 0, Macro::kNoArgs);

	s->Define(m);
}


void Compiler::Undefine(const char *name)
{
	Symbol::Get(name)->Undefine();
}


void Compiler::ReleaseBuffers()
{
	for(size_t i=0; i<fBuffers.size(); i++)
		delete fBuffers[i];


	fBuffers.resize(0);
}


long Compiler::Print(RCX_Printer *dst, short index, long start, long end)
{
	// sanity check for buffers
	if (index < 0 || index >= (short)fBuffers.size()) return end;

	const Buffer *b = fBuffers[index];

	start = b->FindStartOfLine(start);
	end = b->FindEndOfLine(end);

	dst->Print(b->GetData() + start, end - start + 1);

	return end;
}
