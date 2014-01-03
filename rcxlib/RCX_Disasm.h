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

#ifndef __RCX_Disasm_h
#define __RCX_Disasm_h

#ifndef __PTypes_h
#include "PTypes.h"
#endif

#ifndef __RCX_Result_h
#include "RCX_Result.h"
#endif

#ifndef __RCX_Target_h
#include "RCX_Target.h"
#endif

#ifndef __RCX_Constants_h
#include "RCX_Constants.h"
#endif


#ifndef _STDIO_H
#include <cstdio>
using std::FILE;
#endif

#include <string>
using std::string;

#include <map>
using std::map;

typedef map<int, string> labels_type;
typedef labels_type::value_type value_type;

class RCX_Printer;
class RCX_SourceFiles;
class RCX_SourceTag;

class RCX_Disasm
{
public:
	RCX_Disasm(RCX_TargetType targetType);

	void	Print(RCX_Printer *dst, bool genLASM, string name,
          RCX_ChunkType type, int ChunkNum, const UByte *code, int length);

	void	Print(RCX_Printer *dst, bool genLASM, string name,
          RCX_ChunkType type, int ChunkNum, const UByte *code, int length,
          RCX_SourceFiles *sf, const RCX_SourceTag *tags, int tagCount);

	RCX_Result	Print1(RCX_Printer *dst, const UByte *code, int length, UShort pc);
        RCX_Result	SPrint1(char *text, const UByte *code, int length, UShort pc);


	// format of internal instruction information
	// the declaration is public so that a global table may be defined
	struct Instruction
	{
		const char*	fName;
		UByte		fOpcode;
		ULong		fArgs;
	};

private:
	void		DefineInstructions(const Instruction** OpArray, const Instruction *inst);
	void 		SPrintValue(char *text, int type, short data);
	void 		SPrintArg(char *text, ULong format, const UByte *code, UShort pc);
	void		SPrintCondition(char *text, const UByte *code);
	const char*	GetTypeName(int type);

        RCX_Result	FindLabel(const UByte *code, int length, UShort pc);
        void		FindLabelArg(ULong format, const UByte *code, UShort pc);
	void		LASMOutputHeader(RCX_Printer *dst, string name, RCX_ChunkType type, int ChunkNum);
	void		LASMOutputFooter(RCX_Printer *dst, RCX_ChunkType type, UShort pc);


	const Instruction*	fOpDispatch[256];
	const Instruction*	fResOpDisp[256];

        labels_type	fLabels;
	RCX_TargetType	fTarget;
        bool		fGenLASM;
        RCX_ChunkType	fCurType;
        int		fCurNum;
};


class RCX_Printer
{
public:
	virtual ~RCX_Printer() {}
	virtual	void Print(const char *text);
	virtual void Print(const char *text, int length) = 0;
};

class RCX_StdioPrinter : public RCX_Printer
{
public:
	RCX_StdioPrinter(FILE *fp)	{ fFile = fp; }
	~RCX_StdioPrinter() {}

	virtual void Print(const char *text)				{ fputs(text, fFile); }
	virtual void Print(const char *text, int length)	{ fwrite(text, length, 1, fFile); }

private:
	FILE*	fFile;
};


class RCX_SourceFiles
{
public:
	virtual ~RCX_SourceFiles() {}
	virtual long Print(RCX_Printer *printer, short index, long start, long end) = 0;
};


#endif
