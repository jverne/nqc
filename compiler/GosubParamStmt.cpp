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
 * The Initial Developer of this code is John Hansen.
 * Portions created by John Hansen are Copyright (C) 2005 John Hansen.
 * All Rights Reserved.
 */

#include "GosubParamStmt.h"
#include "Bytecode.h"
#include "AtomExpr.h"
#include "Fragment.h"
#include "Error.h"
#include "RCX_Cmd.h"

void GosubParamStmt::AddParams(vector<Expr*> &params)
{
        for(size_t i=0; i<params.size(); i++)
                fParams.push_back(params[i]);
}


void GosubParamStmt::EmitActual(Bytecode &b)
{
  // subroutines with parameters do not use the variable allocator for its parameters
  // instead they use the stack
        if (!b.GetVarAllocator().CheckLocalMask(fFragment->GetLocalMask()))
	{
		Error(kErr_NoMoreVars).Raise(&fLocation);
		return;
	}

        size_t argCount = fFragment->GetArgCount();

        if (argCount != fParams.size())
        {
                Error(kErr_ParamCount).Raise(&fLocation);
                return;
        }

	RCX_Cmd cmd;

        // push parameters onto the stack
        for(int i = 0; i < (int)fParams.size(); i++)
        {
          RCX_Value ea = fParams[i]->EmitAny(b);
          cmd.MakePushStackEntry(ea);
          b.Add(cmd);
          b.ReleaseTempEA(ea);
        }

	cmd.Set(kRCX_GoSubOp, fFragment->GetNumber());
	b.Add(cmd);

        if (fFragment->GetArgCount() > 0)
        {
          // pop parameters off the stack
          cmd.Set(kRCX_PopStackEntryOp, fParams.size());
          b.Add(cmd);
        }

}


Stmt* GosubParamStmt::CloneActual(Mapping *b) const
{
        GosubParamStmt *g = new GosubParamStmt(fFragment, fLocation);
        for(size_t i=0; i<fParams.size(); i++)
                g->fParams.push_back(fParams[i]->Clone(b));
	return g;
}
