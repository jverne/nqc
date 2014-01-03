%{
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

#include <string.h>
#include <stdio.h>
#include "Symbol.h"
#include "RCX_Constants.h"
#include "JumpStmt.h"
#include "Buffer.h"
#include "parser.h"
#include "Compiler.h"
#include "Error.h"
#include "Bytecode.h"


#define kMaxFileDepth	16
#define kMaxFileCount	255

typedef struct InputFile
{
	struct InputFile	*fNext;
	int		fSourceIndex;
	YY_BUFFER_STATE	fBufferState;
	long		fSavedOffset;
	const char*	fDataPtr;
	int			fDataRemain;
} InputFile;

static int sFileDepth = 0;
static InputFile *sCurrentInputFile = 0;
static int sSourceIndex = 0;
static long sOffset = 0;
static int sReturnWhitespace = 0;
static int sInsideDirective = 0;
//static int sInsideProse = 0;

static int FillBuffer(char *ptr, int max);

#define YY_DECL int yylex(YYSTYPE &yylval)
#define YY_USER_ACTION	{ sOffset += yyleng; }
#define YY_INPUT(buf,res,max)	(res = FillBuffer(buf, max))

#define Return(tok, val) do { yylval.fInt = val; return tok; } while(0)

// this is a hack so that isatty() and fileno() aren't required for the Win32/macintosh builds
#if defined(WIN32) || defined(macintosh)
#define YY_NEVER_INTERACTIVE 1
#endif

// Unused defs
#define YY_NO_INPUT 1


%}

%option nounput
%option outfile="lexer.cpp"

id0			[a-zA-Z_]
idn			[a-zA-Z_0-9]
digit		[0-9]
hex			[0-9a-fA-F]
any			[^\r\n]
ws			[ \t]
nl			[\r\n]

%x COMMENT PREPROC

%%


"//"{any}*	;
\032	; // hack for DOS EOF characters

{nl}				{ if (sInsideDirective) { sInsideDirective = 0; return NL; } }
\\{nl}				{ }


{ws}*"##"			{ return PP_GLOM; }
{ws}*#{ws}*			{ if (sInsideDirective) return '#'; else { BEGIN(PREPROC); sInsideDirective = 1; } }
<PREPROC>"include"	{ BEGIN(INITIAL); return PP_INCLUDE; }
<PREPROC>"define"	{ BEGIN(INITIAL); return PP_DEFINE; }
<PREPROC>"ifdef"	{ BEGIN(INITIAL); Return(PP_IFDEF, true); }
<PREPROC>"ifndef"	{ BEGIN(INITIAL); Return(PP_IFDEF, false); }
<PREPROC>"if"		{ BEGIN(INITIAL); return PP_IF; }
<PREPROC>"else"		{ BEGIN(INITIAL); return PP_ELSE; }
<PREPROC>"elif"		{ BEGIN(INITIAL); return PP_ELIF; }
<PREPROC>"endif"	{ BEGIN(INITIAL); return PP_ENDIF; }
<PREPROC>"undef"	{ BEGIN(INITIAL); return PP_UNDEF; }
<PREPROC>"pragma"	{ BEGIN(INITIAL); return PP_PRAGMA; }
<PREPROC>"error"	{ BEGIN(INITIAL); return PP_ERROR; }
<PREPROC>"warning"	{ BEGIN(INITIAL); return PP_WARNING; }
<PREPROC>{nl}		{ BEGIN(INITIAL); yyless(yyleng-1); return PP_UNKNOWN; }
<PREPROC>.			{ BEGIN(INITIAL); return PP_UNKNOWN; }

"if"			{ return IF; }
"else"			{ return ELSE; }
"while"			{ return WHILE; }
"do"			{ return DO; }
"for"			{ return FOR; }
"repeat"		{ return REPEAT; }
"break"			{ yylval.fInt = Bytecode::kBreakFlow; return JUMP; }
"continue"		{ yylval.fInt = Bytecode::kContinueFlow; return JUMP; }
"return"		{ yylval.fInt = Bytecode::kReturnFlow; return JUMP; }
"switch"		{ return SWITCH; }
"case"			{ return CASE; }
"default"		{ return DEFAULT; }
"monitor"		{ return MONITOR; }
"acquire"		{ return ACQUIRE; }
"catch"			{ return CATCH; }
"goto"			{ return GOTO; }

"int"			{ return INT; }
"void"			{ return T_VOID; }
"const"			{ return T_CONST; }
"__sensor"		{ return SENSOR; }
"__type"		{ return TYPE; }
"__event_src"	{ return EVENT_SRC; }
"__taskid"		{ return TASKID; }
"__nolist"		{ return NOLIST; }
"__res"			{ return RES; }
"asm"			{ return ASM; }
"task"			{ return TASK; }
"sub"			{ return SUB; }
"stop"			{ Return( TASKOP, kRCX_StopTaskOp); }
"start"			{ Return( TASKOP, kRCX_StartTaskOp); }
"abs"			{ return ABS; }
"sign"			{ return SIGN; }

"+="			{ Return( ASSIGN, kRCX_AddVar); }
"-="			{ Return( ASSIGN, kRCX_SubVar); }
"*="			{ Return( ASSIGN, kRCX_MulVar); }
"/="			{ Return( ASSIGN, kRCX_DivVar); }
"&="			{ Return( ASSIGN, kRCX_AndVar); }
"|="			{ Return( ASSIGN, kRCX_OrVar); }
"||="			{ Return( ASSIGN, kRCX_AbsVar); }
"+-="			{ Return( ASSIGN, kRCX_SgnVar); }

">>="			{ Return( ASSIGN2, RIGHT); }
"<<="			{ Return( ASSIGN2, LEFT); }
"%="			{ Return( ASSIGN2, '%'); }
"^="			{ Return( ASSIGN2, '^'); }

"=="			{ return REL_EQ; }
"!="			{ return REL_NE; }
"<="			{ return REL_LE; }
">="			{ return REL_GE; }

"&&"			{ return AND; }
"||"			{ return OR; }

"++"			{ Return( INCDEC, 1); }
"--"			{ Return( INCDEC, 0); }

"true"			{ return CTRUE; }
"false"			{ return CFALSE; }

"<<"			{ return LEFT; }
">>"			{ return RIGHT; }

"@@"			{ return INDIRECT; }

{id0}{idn}*		{ yylval.fSymbol = Symbol::Get(yytext); return ID; }
0[xX]{hex}+		{ char*ptr; yylval.fInt = strtol(yytext, &ptr, 0); return NUMBER; }
{digit}+		{ yylval.fInt = (int)atof(yytext); return NUMBER; }

"\""[^\"]*"\""		{ yytext[yyleng-1]=0; yylval.fString = yytext+1; return STRING; }

{ws}+			{ if (sReturnWhitespace) return WS; }


.				{ return yytext[0]; }

"/*"         BEGIN(COMMENT);
<COMMENT>[^*\r\n]*			/* eat anything that's not a '*' */
<COMMENT>"*"+[^*/\r\n]*		/* eat up '*'s not followed by '/'s */
<COMMENT>{nl}				/* eat up newlines */
<COMMENT>"*"+"/"        BEGIN(INITIAL);


%%


void LexCurrentLocation(LexLocation &loc)
{
	loc.fLength = yyleng;
	loc.fIndex = sSourceIndex;
	loc.fOffset = sOffset - yyleng;
}

void LexReturnWhitespace(int mode)
{
	sReturnWhitespace = mode;
}


int LexFindAndPushFile(const char *name)
{
	Buffer *b = Compiler::Get()->CreateBuffer(name);
	if (!b)
	{
		Error(kErr_FileOpen, name).RaiseLex();
		return 0;
	}
	return LexPush(b);
}


int LexPush(Buffer *b)
{
	InputFile *inputFile;
	int index;

	index = Compiler::Get()->AddBuffer(b);

	// make sure max file depth and file count haven't been exceeded
	if (sFileDepth == kMaxFileDepth ||
		index > kMaxFileCount) return 0;

	// save line number and buffer in previous file
	if (sCurrentInputFile)
	{
		sCurrentInputFile->fSavedOffset = sOffset;
		sCurrentInputFile->fBufferState = YY_CURRENT_BUFFER;
	}

	inputFile = (InputFile *)malloc(sizeof(InputFile));
	inputFile->fBufferState = yy_create_buffer(0, YY_BUF_SIZE);
	inputFile->fDataPtr = b->GetData();
	inputFile->fDataRemain = b->GetLength();
	inputFile->fSourceIndex = index;

	// link into list
	inputFile->fNext = sCurrentInputFile;
	sCurrentInputFile = inputFile;
	sFileDepth++;

	// switch to new buffer
	sOffset = 0;
	sSourceIndex = index;
	yy_switch_to_buffer(inputFile->fBufferState);

	return 1;
}


int yywrap()
{
	InputFile *inputFile;

	// check for unterminated comments
	if (YY_START == COMMENT)
	{
		Error(kErr_UnterminatedComment).RaiseLex();
		BEGIN(INITIAL);
	}

	// if no input files, just return
	if (sCurrentInputFile == 0) return 1;

	// pop an input file off the list
	inputFile = sCurrentInputFile;
	sCurrentInputFile = inputFile->fNext;
	sFileDepth--;

	// cleanup the input file
	yy_delete_buffer(YY_CURRENT_BUFFER);
	free(inputFile);

	// if no more files, just return 1
	if (!sCurrentInputFile) return 1;

	// switch to next file
	yy_switch_to_buffer(sCurrentInputFile->fBufferState);
	sOffset = sCurrentInputFile->fSavedOffset;
	sSourceIndex = sCurrentInputFile->fSourceIndex;

	// tell yylex() to continue
	return 0;
}


void LexReset()
{
	while(yywrap() == 0)
		;
}

int FillBuffer(char *buf, int max_size)
{
	int result;
	// if no files are pending, return 0 (EOF)
	if (!sCurrentInputFile) return 0;

	int n = sCurrentInputFile->fDataRemain;
	if (n > max_size)
		n = max_size;

	memcpy(buf, sCurrentInputFile->fDataPtr, n);
	sCurrentInputFile->fDataPtr += n;
	sCurrentInputFile->fDataRemain -= n;

	result = n;
	return result;
}
