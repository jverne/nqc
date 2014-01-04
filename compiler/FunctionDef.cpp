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

#include "FunctionDef.h"
#include "Stmt.h"
#include "Scope.h"
#include "Program.h"

FunctionDef::FunctionDef()
{
	fBody = 0;
	fName = 0;
	fExpanded = false;
	fListingEnabled = true;
}


FunctionDef::~FunctionDef()
{
	delete fBody;
}


bool FunctionDef::AddArg(const Symbol *name, ArgType type)
{
	Arg a;

	for(size_t i=0; i<fArgs.size(); i++)
	{
		if (fArgs[i].fName == name) return false;
	}

	a.fName = name;
	a.fType = type;

	fArgs.push_back(a);
	return true;
}


void FunctionDef::SetBody(Stmt *s)
{
	delete fBody;
	fBody = s;
}




void FunctionDef::CreateArgVars()
{
	for(size_t i=0; i<fArgs.size(); i++)
	{
		int var;

		var = gProgram->NextVirtualVar();

		switch(fArgs[i].fType)
		{
			case kConstantArg:
				var |= kVirtualConstantFlag | kVirtualReadOnlyFlag;
				break;
			case kConstRefArg:
			case kSensorArg:
				var |= kVirtualReadOnlyFlag;
				break;
                        case kPointerArg:
                                var |= kPointerFlag;
                                break;
                        case kConstPtrArg:
                                var |= kVirtualReadOnlyFlag | kPointerFlag;
                                break;
			default:
				break;
		}

		fArgs[i].fVar = var;
		gProgram->DefineVar(fArgs[i].fName, var, false,
                  ((fArgs[i].fType == kPointerArg) || (fArgs[i].fType == kConstPtrArg)),
                  false);
	}
}


void FunctionDef::SetLocations(LocationNode *start, LocationNode *end)
{
	fStart = start->GetLoc();
	delete start;

	fEnd = end->GetLoc();
	delete end;
}

