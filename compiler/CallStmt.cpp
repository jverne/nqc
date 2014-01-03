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


#include "CallStmt.h"
#include "Bytecode.h"
#include "Program.h"
#include "Symbol.h"
#include "Fragment.h"
#include "GosubStmt.h"
#include "GosubParamStmt.h"
#include "InlineStmt.h"
#include "FunctionDef.h"
#include "Mapping.h"
#include "AtomExpr.h"
#include "DerefExpr.h"
#include "DeclareStmt.h"
#include "Error.h"
#include "BlockStmt.h"
#include "ScopeStmt.h"
#include "Resource.h"

CallStmt::CallStmt()
{
	fName = 0;
}


CallStmt::~CallStmt()
{
	for(size_t i=0; i<fParams.size(); i++)
		delete fParams[i];

}


void CallStmt::Expand(Fragment *fragment)
{
	if (Fragment *sub = gProgram->GetSub(fName))
	{
                const RCX_Target *t = gProgram->GetTarget();
                if (!t->fSubParams)
                {
                        // sub call
                        if (!fragment->IsTask())
                        {
                                Error(kErr_NestedSubs).Raise(&fLocation);
                        }
                        else if (fParams.size() != 0)
                        {
                                Error(kErr_ParamCount).Raise(&fLocation);
                        }
                        else
                        {
                                SetBody(new GosubStmt(sub, fLocation));
                                sub->AssignTaskID(fragment->GetTaskID());
                        }
                }
                else
                {
                        GosubParamStmt *g = new GosubParamStmt(sub, fLocation);
                        g->AddParams(fParams);
                        SetBody(g);
                        sub->AssignTaskID(fragment->GetTaskID());
                }
	}
	else if (FunctionDef *func = gProgram->GetFunction(fName))
	{
		if (func->IsExpanded())
		{
			Error(kErr_RecursiveCall, fName->GetKey()).Raise(&fLocation);
		}
		else
		{
			func->SetExpanded(true);
			ExpandFunction(func, fragment);
			func->SetExpanded(false);
		}
	}
	else
	{
		Error(kErr_UndefinedFunction, fName->GetKey()).Raise(&fLocation);
	}
}


void CallStmt::EmitActual(Bytecode &b)
{
	Stmt *body = GetBody();
	if (body)
		body->Emit(b);
}

Stmt *CallStmt::CloneActual(Mapping *b) const
{
	CallStmt *c = new CallStmt;

	c->fName = fName;
	c->fLocation = fLocation;
	const Stmt *body = GetBody();
	c->SetBody(body ? body->Clone(b) : 0);

	for(size_t i=0; i<fParams.size(); i++)
		c->fParams.push_back(fParams[i]->Clone(b));

	return c;
}

void CallStmt::ExpandFunction(FunctionDef *func, Fragment *fragment)
{
	size_t argCount = func->GetArgCount();

	// check number of parameters
	if (argCount != fParams.size())
	{
		Error(kErr_ParamCount).Raise(&fLocation);
		return;
	}

	/* statement should look like this:
	 *
	 * CallStmt
	 *   |
	 * InlineStmt
	 *   |
	 * ScopeStmt
	 *   |
	 * BlockStmt
	 *   /        |       \
	 * DeclareStmt...   body of function
	 */
	BlockStmt *block = new BlockStmt();
	SetBody( new InlineStmt(new ScopeStmt(block), func));

	Mapping mapping;

	for(size_t i=0; i<argCount; i++)
	{
		const Expr* arg = fParams[i];
		int var = func->GetArgVar(i);
		int val;

		switch(func->GetArgType(i))
		{
			case FunctionDef::kConstantArg:
				if (!arg->Evaluate(val))
				{
					Error(kErr_ParamType, "constant").Raise(&arg->GetLoc());
					return;
				}
				mapping.Add(var, new AtomExpr(kRCX_ConstantType, val, fLocation));
				break;
			case FunctionDef::kIntegerArg:
				val = gProgram->NextVirtualVar();
				mapping.Add(var, new AtomExpr(kRCX_VariableType, val, fLocation));
				{
					DeclareStmt *ds = new DeclareStmt(func->GetArgName(i), val, fLocation, 1, false, true);
					ds->SetInitialValue(arg->Clone(0));
					block->Add(ds);
				}
				break;
			case FunctionDef::kReferenceArg:
				val = arg->GetLValue();
				if (val == kIllegalVar)
				{
					Error(kErr_ParamType, "variable").Raise(&arg->GetLoc());
					return;
				}
				mapping.Add(var, new AtomExpr(kRCX_VariableType, val, fLocation));
				break;
			case FunctionDef::kConstRefArg:
				mapping.Add(var, arg->Clone(0));
				break;
			case FunctionDef::kSensorArg:
				if (RCX_VALUE_TYPE(arg->GetStaticEA()) != kRCX_InputValueType)
				{
					Error(kErr_ParamType, "sensor").Raise(&arg->GetLoc());
					return;
				}
				mapping.Add(var, arg->Clone(0));
				break;
                        case FunctionDef::kPointerArg:
                                if (!arg->LValueIsPointer())
                                {
                                    Error(kErr_ParamType, "pointer").Raise(&arg->GetLoc());
                                    return;
                                }
                                mapping.Add(var, arg->Clone(0));
                                break;
                        case FunctionDef::kConstPtrArg:
                                if (!arg->LValueIsPointer())
                                {
                                    Error(kErr_ParamType, "pointer").Raise(&arg->GetLoc());
                                    return;
                                }
                                val = gProgram->NextVirtualVar();
                                {
                                    DeclareStmt *ds = new DeclareStmt(func->GetArgName(i), val, fLocation, 1, true, true);
                                    ds->SetInitialValue(arg->Clone(0));
                                    block->Add(ds);
                                }
                                mapping.Add(var, new AtomExpr(kRCX_VariableType, val, fLocation, true));
                                break;
			default:
				Error(kErr_ParamType, "???").Raise(&fParams[i]->GetLoc());
				return;
		}
	}


	// add body of inline and then expand
	block->Add(func->GetBody()->Clone(&mapping));

	Expander e(fragment);
	Apply(GetBody(), e);
}


void CallStmt::GetExprs(vector<Expr *> &v) const
{
	int n = fParams.size();

	for(int i=0; i<n; ++i)
		v.push_back(fParams[i]);
}



bool CallStmt::Expander::operator()(Stmt *s)
{
	CallStmt *cs;

	if ((cs=dynamic_cast<CallStmt*>(s)) != 0)
	{
		cs->Expand(fFragment);
		return false;
	}

	return true;
}

