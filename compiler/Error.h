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
 * Portions created by David Baum are Copyright (C) 1999 David Baum.
 * All Rights Reserved.
 *
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 *
 */

#ifndef __Error_h
#define __Error_h

#include <stdint.h>

enum ErrorCode
{
	kErr_None = 0,
	kErr_Parser,
	kErr_UnterminatedIfdef,
	kErr_UnexpectedElse,
	kErr_UnknownDirective,
	kErr_IncludeNeedsName,
	kErr_FileOpen,
	kErr_SymbolNameNeeded,
	kErr_DefineRedef,
	kErr_DefineArgs,
	kErr_CircularDef,
	kErr_WrongArgCount,
	kErr_CondExpression,
	kErr_UnexpectedEOL,
	kErr_UnbalancedParens,
	kErr_UnterminatedComment,

	kErr_SymRedef,
	kErr_NoVarDecl,
	kErr_NoMoreVars,
	kErr_Undeclared,
	kErr_ConstantNeeded,
	kErr_LValueNeeded,

	kErr_UndefinedTask,
	kErr_TooManySubs,
	kErr_TooManyTasks,
	kErr_UnknownInit,
	kErr_UndefinedMain,
	kErr_NoContinueContext,
	kErr_NoSwitch,
	kErr_NoBreakContext,
	kErr_BadExpression,
	kErr_NoMoreTemps,
	kErr_NestedSubs,
	kErr_ParamCount,
	kErr_UndefinedFunction,
	kErr_ParamType,
	kErr_UndefinedVar,

	kErr_CaseRange,
	kErr_DuplicateCase,
	kErr_RecursiveCall,
	kErr_NumberRange,

	kErr_NoTargetResources,
	kErr_NoTargetEvents,
	kErr_NoTargetArrays,
	kErr_NoTargetPartialCatch,
        kErr_NoTargetIndirect,
        kErr_NoTargetSubParams,

	kErr_BadEventSource,
	kErr_NoNestedResources,
	kErr_NoNestedEvents,
	kErr_CouldNotReserveVars,

        kErr_VarIsNotPointer,
	kErr_VarIsArray,
	kErr_VarIsNotArray,
	kErr_TaskIdUnknown,
	kErr_ErrorDirective,

	kErr_UndefinedLabel,
	kErr_RedefinedLabel,

	kErr_BadResourceType,
	kErr_TooManyResources,

        kErr_AddrOfNonConstantIndex,

	// catch-all for things in progress
	kErr_NotSupported,

	// warnings
	kWarn_Base,
	kWarn_IllegalPragma,
	kWarn_WarningDirective,

	kErr_Last
};


class ErrorHandler;
struct LexLocation;

class Error
{
public:
	enum
	{
		kMaxErrorMsg = 256
	};

	Error(ErrorCode code, intptr_t data=0) : fCode(code), fData(data) {}
	Error(ErrorCode code, const char *s) : fCode(code), fData((intptr_t)s) {}

	// raising errors
	void	Raise(const LexLocation *loc) const;	// raise error at specified location
	void	RaiseLex() const;						// raise error at current lex location

	// useful for error handlers
	ErrorCode	GetCode() const	{ return fCode; }
	intptr_t	GetData() const	{ return fData; }
	bool		IsWarning() const;
	void		SPrint(char *str) const;

private:
	ErrorCode	fCode;
	intptr_t	fData;
};


class ErrorHandler
{
public:
			void	Raise(const Error&e, const LexLocation *loc);
			void	Reset();
			int	GetErrorCount()	{ return fErrorCount; }
			int	GetWarningCount()	{ return fWarningCount; }

	virtual void	ClearErrors();
	virtual void	AddError(const Error &e, const LexLocation *loc) = 0;

	static ErrorHandler*	Get()	{ return sErrorHandler; }

protected:
			ErrorHandler()	{ sErrorHandler = this; }
private:
	int		fErrorCount;
	int		fWarningCount;

	static ErrorHandler*	sErrorHandler;
};

#endif
