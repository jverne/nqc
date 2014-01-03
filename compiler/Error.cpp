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

#include <cstdio>
#include "parser.h"
#include "Buffer.h"
#include "Error.h"
#include "LexLocation.h"

using std::sprintf;

static const char *sErrorText[] = {
	// errors
	"internal error",
	"%s",
	"unterminated #if/#ifdef/#ifndef",
	"%s without preceeding #if/#ifdef/#ifndef",
	"illegal preprocessor directive",
	"#include requires a filename",
	"could not open file %s",
	"%s required a symbol name",
	"redefinition of \'%s\'",
	"illegal define arguments",
	"circular definition in \'%s\'",
	"wrong number of arguments to \'%s\'",
	"syntax error in expression",
	"unexpected end of line",
	"unbalanced parentheses",
	"unterminated comment",

	"\'%s\' cannot be redeclared",
	"variable declarations not allowed in subs",
	"could not allocate storage for variable",
	"\'%s\' has not been declared",
	"constant expression required",
	"expression is not a legal target for assignment",

	"undefined task \'%s\'",
	"maximum of %d subroutines exceeded",
	"maximum of %d tasks exceeded",
	"initialization function \'%s\' is not defined",
	"task \'main\' not defined",
	"\'continue\' is only valid within a loop",
	"case/default only valid in a switch",
	"\'break\' is only valid in a switch or loop",
	"Expression is not a valid argument type",
	"Ran out of temporary variables",
	"nested subroutine calls are not allowed",
	"wrong number of parameters",
	"undefined function \'%s\'",
	"argument must be a %s",
	"undefined variable \'%s\'",

	"case value must be between -32768 and 32767",
	"duplicate case or default label in switch",
	"recursive function call: \'%s\'",
	"number out of range",

	"%s does not support resource acquisition",
	"%s does not support event monitoring",
	"%s does not support arrays",
        "%s does not support partial catch",
        "%s does not support indirect variables",
        "%s does not support subroutine parameters",

	"illegal event source",
	"nested resource acquisition is not allowed",
	"nested event monitoring is not allowed",
	"could not reserve variables",

        "%s is not a pointer",
	"%s is an array",
	"%s is not an array",
	"sub called by multiple tasks - __taskid cannot be determined",
	"preprocessor #error directive",

	"undefined label \'%s\'",
	"label \'%s\' redefined",

	"illegal resource type",
	"maximum of %d resources of this type exceeded",

        "cannot take the address of an array with a non-constant index",

	// catch-all for things in progress
	"%s is not yet supported",

	// warnings
	"Unknown Warning",
	"illegal #pragma",
	"preprocessor #warning directive",
};

ErrorHandler* ErrorHandler::sErrorHandler = 0;


void Error::RaiseLex() const
{
	LexLocation loc;

	LexCurrentLocation(loc);
	Raise(&loc);
}


void Error::Raise(const LexLocation *loc) const
{
	ErrorHandler* eh = ErrorHandler::Get();

	if (eh) eh->Raise(*this, loc);
}


bool Error::IsWarning() const
{
	return fCode >= kWarn_Base;
}



void Error::SPrint(char *str) const
{
	if (fCode < 0 || fCode >= (ErrorCode)(sizeof(sErrorText) / sizeof(const char *)))
		sprintf(str, "internal error #%d", fCode);
	else
		sprintf(str, sErrorText[fCode], fData);
}


void ErrorHandler::Reset()
{
	fErrorCount = 0;
	fWarningCount = 0;
	ClearErrors();
}


void ErrorHandler::Raise(const Error &e, const LexLocation *loc)
{
	if (e.IsWarning())
		fWarningCount++;
	else
		fErrorCount++;
	AddError(e, loc);
}

void ErrorHandler::ClearErrors()
{
}
