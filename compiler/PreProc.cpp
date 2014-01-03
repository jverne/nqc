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

/*
 * PreProc reads tokens from the Lexer and emits a preprocessed token stream.
 * The entire process is 'pulled' by calling PreProc::Get(), which in turn calls
 * down through the layers of the Preprocessor:
 *
 * Get() - dispatches directives (DoInclude(), DoDefine(), etc)
 *		When processing a directive, GetRawToken() is called - in effect, macro
 *		substitution happens on playback, not record.
 *
 * GetReplacedToken() - matches ID tokens against macros, and pushes the macro
 *		definition when a macro is used.  Use this call when macro calls should
 *		be expanded.
 *
 * GetRawToken() - maintains the queue of expansions, subsitutes arguments during
 *		expansion, manages the lexer (calls yylex(), maintains fNLRead, etc)
 *
 */

#include <cstring>
#include "PreProc.h"
#include "Symbol.h"
#include "Macro.h"
#include "Expansion.h"
#include "Program.h"
#include "Error.h"

// make sure we get an error if we try to use yylval
#define yylval

// values for argCount
#define kErrorArgs -2


PreProc *gPreProc = new PreProc();


PreProc::PreProc()
{
	fActive = fConditional.IsActive();
	fEndOfFiles = false;
}


PreProc::~PreProc()
{
}


int PreProc::Get(TokenVal &v)
{
	int t;
	long x;

	// read token from lexer, and process any preprocessor commands;
	while(1)
	{
		t = GetReplacedToken(v);

		// strip newlines
		if (t==NL) continue;

		fNLRead = false;

		// check for end
		if (t==0)
		{
			if (!fConditional.IsTopLevel())
				Error(kErr_UnterminatedIfdef).RaiseLex();
			return 0;
		}

		// preprocessor commands
		switch(t)
		{
			case PP_INCLUDE:
				if (fActive)
					DoInclude();
				break;
			case PP_DEFINE:
				if (fActive)
					DoDefine();
				DiscardLine();
				break;
			case PP_IFDEF:
				DoIfdef((bool)v.fInt);
				fActive = fConditional.IsActive();
				DiscardLine();
				break;
			case PP_IF:
				fParser.Parse(x);
				fConditional.If(x ? true : false);
				fActive = fConditional.IsActive();
				DiscardLine();
				break;

			case PP_ELIF:
				fActive=true;
				fParser.Parse(x);
				if (!fConditional.Elif(x ? true : false))
					Error(kErr_UnexpectedElse, "#elif").RaiseLex();
				else
					fActive = fConditional.IsActive();
				DiscardLine();
				break;

			case PP_ELSE:
				if (!fConditional.Else())
					Error(kErr_UnexpectedElse, "#else").RaiseLex();
				else
					fActive = fConditional.IsActive();
				DiscardLine();
				break;
			case PP_ENDIF:
				if (!fConditional.Endif())
					Error(kErr_UnexpectedElse, "#endif").RaiseLex();
				else
					fActive = fConditional.IsActive();
				DiscardLine();
				break;
			case PP_UNDEF:
				DoUndef();
				DiscardLine();
				break;
			case PP_PRAGMA:
				if (fActive)
				{
					if (!DoPragma())
						Error(kWarn_IllegalPragma).RaiseLex();
				}
				DiscardLine();
				break;
			case PP_ERROR:
				Error(kErr_ErrorDirective).RaiseLex();
				DiscardLine();
				break;
			case PP_WARNING:
				Error(kWarn_WarningDirective).RaiseLex();
				DiscardLine();
				break;
			case PP_UNKNOWN:
				if (fActive)
					Error(kErr_UnknownDirective).RaiseLex();
				DiscardLine();
				break;
			default:
				if (fActive) return t;
				break;
		}
	}
}


int PreProc::GetReplacedToken(TokenVal &v)
{
	int t;

	while(1)
	{
		t = GetRawToken(v);

		// macro substitution
		if (t==ID && (v.fSymbol->IsDefined()) && fActive)
		{
			BeginExpansion(v.fSymbol);
		}
		else
			return t;
	}
}



int PreProc::GetRawToken(TokenVal &v)
{
	int t;

	while(1)
	{
		Expansion *e = fExpList.GetHead();

		if (e)
		{
			if (e->IsDone())
			{
				fExpList.RemoveHead();
				delete e;
			}
			else
			{
				t = e->NextToken(v);
				if (t == PP_ARG)
				{
					fExpList.InsertHead(new Expansion(e, v.fInt));
				}
				else
					return t;
			}
		}
		else
		{
			if (fEndOfFiles)
			{
				return 0;
			}

			t = yylex(v);

			if (t==NL)
				fNLRead = true;
			else if (t==0)
				fEndOfFiles = true;

			return t;
		}
	}
}


bool PreProc::DoInclude()
{
	TokenVal v;

	int t = GetRawToken(v);
	DiscardLine();

	if (t != STRING)
	{
		Error(kErr_IncludeNeedsName).RaiseLex();
		return false;
	}

	if (!LexFindAndPushFile(v.fString))
	{
		Error(kErr_FileOpen, v.fString);
		return false;
	}

	return true;
}


bool PreProc::DoDefine()
{
	Symbol *s;
	TokenVal v;
	int t;
	int argCount;

	t = GetRawToken(v);
	if (t != ID)
	{
		Error(kErr_SymbolNameNeeded, "#define").RaiseLex();
		return false;
	}

	s = v.fSymbol;
	if (s->IsDefined())
	{
		Error(kErr_DefineRedef, s->GetKey()).RaiseLex();
		return false;
	}

	argCount = ReadDefineArgs();
	if (argCount == kErrorArgs) return false;

	// hack to detect empty macro
	if (fNLRead)
	{
		s->Define(new Macro(0, nil, -1));
		return true;
	}

	ReadDefineBody();

	s->Define(new Macro(&fTokenBuf[0], fTokenBuf.size(), argCount));
	return true;
}


void PreProc::ReadDefineBody()
{
	int t;
	TokenVal v;
	int count = 0;
	fTokenBuf.resize(0);
	int argnum;

	while(1)
	{
		t = GetRawToken(v);

		switch(t)
		{
			case 0:
			case NL:
				return;
			case ID:
				argnum = MatchArg(v.fSymbol);
				if (argnum != -1)
				{
					t = PP_ARG;
					v.fInt = argnum;
				}
				break;

		}

		fTokenBuf.resize(count+1);
		fTokenBuf[count].fType = t;
		fTokenBuf[count].fValue = v;
		count++;
	}
}


int PreProc::MatchArg(const Symbol *s)
{
	size_t i;

	for(i=0; i<fArguments.size(); i++)
		if (fArguments[i] == s) return i;

	return -1;
}


int PreProc::ReadDefineArgs()
{
	int t;
	TokenVal v;

	fArguments.resize(0);

	// check for space or '('
	LexReturnWhitespace(1);
	t = GetRawToken(v);
	LexReturnWhitespace(0);

	switch(t)
	{
		case WS:
		case NL:
			return Macro::kNoArgs;
                case '(':
			break;
		default:
			Error(kErr_DefineArgs).RaiseLex();
			return kErrorArgs;
	}

	while(1)
	{
		t = GetRawToken(v);

		// check for empty arg list
		if (t==')' && fArguments.size()==0) return 0;

		if (t != ID)
		{
			if (t != NL) DiscardLine();
			return kErrorArgs;
		}

		fArguments.push_back(v.fSymbol);

		t = GetRawToken(v);
		if (t == ')')
			return fArguments.size();
		else if (t != ',')
		{
			if (t != NL) DiscardLine();
			return kErrorArgs;
		}
	}
}


bool PreProc::BeginExpansion(Symbol *s)
{
	Expansion *e;
	Macro *def = s->GetDefinition();

	if (def->IsMarked())
	{
		Error(kErr_CircularDef, s->GetKey()).RaiseLex();
		return false;
	}

	e = new Expansion(def);

	if (!ReadExpansionArgs(e))
	{
		delete e;
		Error(kErr_WrongArgCount, s->GetKey()).RaiseLex();
		return false;
	}

	fExpList.InsertHead(e);
	return true;
}


bool PreProc::ReadExpansionArgs(Expansion *e)
{
	int argCount = e->GetArgCount();
	int t;
	TokenVal v;

	if (argCount == Macro::kNoArgs) return true;

	t = GetRawToken(v);
	if (t != '(') return false;

	for(int i=0; i<argCount-1; i++)
		if (!ReadExpansionArg(e, i, ',')) return false;

	if (argCount == 0)
	{
		t = GetRawToken(v);
		if (t != ')') return false;
	}
	else
		if (!ReadExpansionArg(e, argCount-1, ')')) return false;

	return true;
}


bool PreProc::ReadExpansionArg(Expansion *e, int i, int delim)
{
	int nesting = 0;
	int count = 0;
	int t;
	TokenVal v;

	fTokenBuf.resize(0);

	while(1)
	{
		t = GetRawToken(v);

		switch(t)
		{
			case 0:
				return false;
			case '(':
				nesting++;
				break;
			case ')':
				if (nesting)
				{
					nesting--;
					break;
				}
				// fall through
			case ',':
				if (nesting) break;
				if (t != delim) return false;
				e->SetArg(i, &fTokenBuf[0], fTokenBuf.size());
				return true;
		}

		fTokenBuf.resize(count+1);
		fTokenBuf[count].fType = t;
		fTokenBuf[count].fValue = v;
		count++;
	}

}


bool PreProc::DoIfdef(bool b)
{
	TokenVal v;
	int t;
	bool defined;

	t = GetRawToken(v);
	if (t != ID)
	{
		Error(kErr_SymbolNameNeeded, "#ifdef / #ifndef").RaiseLex();
		return false;
	}

	defined = v.fSymbol->IsDefined();
	fConditional.If(b == defined);

	return true;
}


bool PreProc::DoUndef()
{
	TokenVal v;
	int t;

	t = GetRawToken(v);
	if (t != ID)
	{
		Error(kErr_SymbolNameNeeded, "#undef").RaiseLex();
		return false;
	}

	v.fSymbol->Undefine();
	return true;
}


bool PreProc::DoPragma()
{
	TokenVal v;

	int t = GetRawToken(v);
	if (t != ID) return false;

	const char *pragma = v.fSymbol->GetKey();

	if (strcmp(pragma, "noinit") == 0)
	{
		gProgram->SetInitName(0, 0);
		return true;
	}
	else if (strcmp(pragma, "init") == 0)
	{
		t = GetRawToken(v);
		if (t != ID) return false;
		LexLocation loc;
		LexCurrentLocation(loc);
		gProgram->SetInitName(v.fSymbol, &loc);
		return true;
	}
	else if (strcmp(pragma, "reserve") == 0)
	{
		int start, end;

		t = GetRawToken(v);
		if (t != NUMBER) return false;
		start = v.fInt;

		t = GetRawToken(v);
		if (t == NUMBER)
			end = v.fInt;
		else if (t == NL)
			end = start;
		else
			return false;

		if (!gProgram->ReserveVars(start, end))
			Error(kErr_CouldNotReserveVars).RaiseLex();

		return true;
	}

	return false;
}


void PreProc::DiscardLine()
{
	TokenVal v;
	int t;

	if (fNLRead) return;

	while((t=GetRawToken(v)) != 0)
	{
		if (t==NL) return;
	}
}
