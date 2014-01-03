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
