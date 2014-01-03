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


#include "parse_util.h"
#include "Expr.h"
#include "parser.h"
#include "AssignStmt.h"
#include "AssignMathStmt.h"
#include "Symbol.h"
#include "GosubStmt.h"
#include "Program.h"
#include "UnaryExpr.h"
#include "BinaryExpr.h"
#include "AtomExpr.h"
#include "ValueExpr.h"
#include "Error.h"
#include "ModExpr.h"
#include "CaseStmt.h"
#include "FunctionDef.h"
#include "AsmStmt.h"
#include "ScopeStmt.h"
#include "AcquireStmt.h"
#include "DeclareStmt.h"
#include "ArrayExpr.h"
#include "TaskIdExpr.h"
#include "IndirectExpr.h"
#include "CatchStmt.h"
#include "ShiftExpr.h"
#include "IncDecExpr.h"
#include "AddrOfExpr.h"
#include "DerefExpr.h"
#include "Resource.h"


// template class to auto-delete an object upon destruction
template <class T> class Deleter
{
public:
	Deleter(T *t) : t_(t) {};
	~Deleter()	{ delete t_; }

private:
	T*			t_;
};



int GetConstantValue(const Expr *e)
{
	Deleter<const Expr> d(e);
	int v = 0;
	bool ok;

	ok = e->Evaluate(v);
	if (!ok)
		Error(kErr_ConstantNeeded).Raise(&e->GetLoc());

	return v;
}


int GetLValue(const Expr *e)
{
	Deleter<const Expr> d(e);

	int lv = e->GetLValue();
	if (lv==kIllegalVar)
		Error(kErr_LValueNeeded).Raise(&e->GetLoc());

	return lv;
}


void CheckLValue(Expr *lval)
{
	if (!lval->PotentialLValue())
		Error(kErr_LValueNeeded).Raise(&lval->GetLoc());
}


Expr *MakeUnaryExpr(int op, Expr *arg)
{
	if (!UnaryExpr::NeedsConstant(op) || arg->PromiseConstant())
		return new UnaryExpr(op, arg);

        // Swan target handles this elsewhere
	if (op == '~') {
		// special case transform ~x  ->  -1 - x
		return new BinaryExpr(
			new AtomExpr(kRCX_ConstantType, -1, arg->GetLoc()),
			'-',
			arg
		);
	}

	Error(kErr_ConstantNeeded).RaiseLex();
	// return a dummy expr so the parse tree is still valid
	return arg;
}


Expr *MakeBinaryExpr(Expr *lhs, int op, Expr *rhs)
{
	if (!BinaryExpr::NeedsConstant(op) ||
		(rhs->PromiseConstant() &&
		 lhs->PromiseConstant()))
	{
		return new BinaryExpr(lhs, op, rhs);
	}

	// modulo is a special case (Swan target handles this elsewhere)
	if (op == '%')
	{
		return new ModExpr(lhs, rhs);
	}

	// shifts are also special cases (Swan target handles these elsewhere)
	if (op == LEFT || op == RIGHT)
	{
		// right argument must be a constant
		if (rhs->PromiseConstant())
		{
			return new ShiftExpr(lhs, rhs, (op==LEFT) ? ShiftExpr::kLeft : ShiftExpr::kRight);
		}

		// if rhs is not constant, this will just fall through to
		// the normal binary expression case, which is an error
	}

	// xor handled by a transform (Swan target handles this elsewhere
	if (op == '^')
	{
		// a^b  ->  (-1 - (a&b)) & (a | b)
		Expr *lhc = lhs->Clone(0);
		Expr *rhc = rhs->Clone(0);

		Expr *nand = new BinaryExpr(
			new AtomExpr(kRCX_ConstantType, -1, lhs->GetLoc()),
			'-',
			new BinaryExpr(
				lhc,
				'&',
				rhc
			)
		);

		return new BinaryExpr(
			nand,
			'&',
			new BinaryExpr(lhs, '|', rhs)
		);
	}


	Error(kErr_ConstantNeeded).RaiseLex();
	// return a dummy expr so the parse tree is still valid
	delete rhs;
	return lhs;
}


Expr *MakeValueExpr(Expr *e)
{
	if (!e->PromiseConstant())
	{
		Error(kErr_ConstantNeeded).RaiseLex();
		// return a dummy expr so the parse tree is still valid
		return e;
	}

	// if e can be evaluated, return AtomExpr, otherwise return ValueExpr
	int value;
	if (e->Evaluate(value))
	{
          Expr *newExpr;
          newExpr = new AtomExpr(RCX_VALUE_TYPE(value), RCX_VALUE_DATA_INT(value), e->GetLoc());
	  delete e;
	  return newExpr;
	}
	else
	  return new ValueExpr(e);
}

int VarCodeToChar(int op)
{
  switch((RCX_VarCode)op)
  {
    case kRCX_AddVar:
            return '+';
    case kRCX_SubVar:
            return '-';
    case kRCX_MulVar:
            return '*';
    case kRCX_DivVar:
            return '/';
    case kRCX_AndVar:
            return '&';
    case kRCX_OrVar:
            return '|';
    case kRCX_NotVar:
            return '~';
    case kRCX_ModVar:
            return '%';
    case kRCX_ShlVar:
            return LEFT;
    case kRCX_ShrVar:
            return RIGHT;
    case kRCX_XOrVar:
            return '^';
    default:
    		break;
  }
  return 0;
}

Stmt *MakeAssignStmt(Expr *lhs, int op, Expr *rhs)
{
    const RCX_Target *t = gProgram->GetTarget();
    if ((t->fType != kRCX_SwanTarget) && (lhs->GetLValue()==kIllegalVar))
    {
      switch((RCX_VarCode)op)
      {
        case kRCX_AbsVar:
          return new AssignStmt(lhs, MakeUnaryExpr(ABS, rhs));
        case kRCX_SgnVar:
          return new AssignStmt(lhs, MakeUnaryExpr(SIGN, rhs));
        default:
          return MakeAssign2Stmt(lhs, VarCodeToChar(op), rhs);
      }
    }
    else
    {
      return new AssignMathStmt(lhs, (RCX_VarCode)op, rhs);
    }
}

Stmt *MakeAssign2Stmt(Expr *lhs, int op, Expr *rhs)
{
        const RCX_Target *t = gProgram->GetTarget();
        if (t->fType == kRCX_SwanTarget)
        {
          // Swan has opcodes for %=, ^=, <<=, and >>=
          RCX_VarCode code;
          switch (op)
          {
            case '%'   : code = kRCX_ModVar; break;
            case '^'   : code = kRCX_XOrVar; break;
            case RIGHT : code = kRCX_ShrVar; break;
            default    : code = kRCX_ShlVar; break;
          }
          return MakeAssignStmt(lhs, code, rhs);
        }
        else
        {
          Expr *dst = lhs->Clone(0);
          Expr *math = MakeBinaryExpr(lhs, op, rhs);

          return new AssignStmt(dst, math);
        }
}

Expr *MakeIncDecExpr(Expr *lhs, bool inc, bool pre, const LexLocation &loc)
{
    return new IncDecExpr(GetLValue(lhs), inc, pre, loc);
}

CaseStmt* MakeCaseStmt(const Expr *e, const struct LexLocation &loc)
{
	int v = GetConstantValue(e);

	if (v < -32768 || v > 32767)
		Error(kErr_CaseRange).RaiseLex();


	return new CaseStmt(v, loc);
}


Stmt *MakeAcquireStmt(const Expr *e, Stmt *body, Stmt *handler, LocationNode *ln)
{
	Deleter<LocationNode> d(ln);
	int v = GetConstantValue(e);

	return new AcquireStmt(v, body, handler, ln->GetLoc());
}


Stmt *MakeCatchStmt(const Expr *e, Stmt *body, LocationNode *ln)
{
	Deleter<LocationNode> d(ln);

	int v;

	if (e)
		v = GetConstantValue(e);
	else
		v = -1;

	return new CatchStmt(v, body, ln->GetLoc());
}


void DefineSubArg(Fragment *f, const Symbol *name, int type)
{
        const RCX_Target *t = gProgram->GetTarget();
        if (!t->fSubParams)
                Error(kErr_NoTargetSubParams, t->fName).RaiseLex();
        if (!f->AddArg(name, (FunctionDef::ArgType)type))
                Error(kErr_SymRedef, name->GetKey()).RaiseLex();
}


void DefineArg(FunctionDef *f, const Symbol *name, int type)
{
        if (!f->AddArg(name, (FunctionDef::ArgType)type))
                Error(kErr_SymRedef, name->GetKey()).RaiseLex();
}


void BeginScope()
{
	gProgram->PushScope();
}


Stmt *EndScope(Stmt *body)
{
	gProgram->PopScope();
	return new ScopeStmt(body);
}


Fragment* CreateSubroutine(Fragment *f, Symbol *name, Stmt *body)
{
        f->SetName(name);
        gProgram->PushScope();
        f->CreateArgVars();
        f->SetBody(body);
        gProgram->PopScope();
//        f->CreateArgVars();
        return f;
}


Fragment* BeginSubWithParams(Fragment *f, Symbol *name)
{
        f->SetName(name);
        gProgram->PushScope();
        f->CreateArgVars();
        return f;
}


void EndSubWithParams(Fragment *f, Stmt *body, LocationNode *start, LocationNode *end)
{
        f->SetLocations(start, end);
        f->SetBody(body);
        gProgram->PopScope();
}


FunctionDef* BeginFunction(FunctionDef *f, const Symbol *name, bool listing)
{
        f->SetName(name);
        gProgram->AddFunction(f);
        gProgram->PushScope();
        f->CreateArgVars();
        f->SetListingEnabled(listing);
        return f;
}


void EndFunction(FunctionDef *f, Stmt *body, LocationNode *start, LocationNode *end)
{
        f->SetLocations(start, end);
        f->SetBody(body);
        gProgram->PopScope();
}


ConstField *MakeConstField(Expr *e)
{
	ConstField *c = new ConstField(e);

	if (!e->PromiseConstant())
	{
		Error(kErr_ConstantNeeded).RaiseLex();
	}
	return c;
}


Expr *MakeVarExpr(const Symbol *name, LocationNode *ln)
{
	Deleter<LocationNode> d(ln);
	bool array, ptr, stack;

	int var = gProgram->GetVar(name, array, ptr, stack);
	if (var == kIllegalVar)
	{
		// check to see if name is for resource
		const Resource *resource = gProgram->GetResource(name);
		if (resource)
		{
			return new AtomExpr(kRCX_ConstantType, resource->GetNumber() , ln->GetLoc());
		}

                Error(kErr_UndefinedVar, name->GetKey()).Raise(&ln->GetLoc());
	}
	else if (array)
	{
		Error(kErr_VarIsArray, name->GetKey()).Raise(&ln->GetLoc());
	}

	return new AtomExpr(kRCX_VariableType, var, ln->GetLoc(), ptr);
}


Expr *MakeIndirectExpr(Expr *src, Expr *idx)
{
	if (!src->PromiseConstant())
	{
		Error(kErr_ConstantNeeded).RaiseLex();
		delete src;
		return idx;
	}

	return new IndirectExpr(src, idx);
}


Expr *MakeTaskIdExpr(LocationNode *ln)
{
	Deleter<LocationNode> d(ln);

	return new TaskIdExpr(ln->GetLoc());
}


Expr *MakeArrayExpr(const Symbol *name, LocationNode *ln, Expr *index)
{
        Deleter<LocationNode> d(ln);
        bool array, ptr, stack;

        int var = gProgram->GetVar(name, array, ptr, stack);
        if (var == kIllegalVar)
        {
                Error(kErr_UndefinedVar, name->GetKey()).Raise(&ln->GetLoc());
        }
        else if (!array)
        {
                Error(kErr_VarIsNotArray, name->GetKey()).Raise(&ln->GetLoc());
       }

        return new ArrayExpr(var, index);
}

Expr *MakeAddrOfExpr(const Symbol *name, LocationNode *ln, Expr * index)
{
  Deleter<LocationNode> d(ln);
  bool array, ptr, stack;
  int value=0;

  int var = gProgram->GetVar(name, array, ptr, stack);
  if (var == kIllegalVar)
  {
          Error(kErr_UndefinedVar, name->GetKey()).Raise(&ln->GetLoc());
  }
  else if (array)
  {
    // if the expression is a constant then we can take the address of it
    if (index)
    {
      if (!index->Evaluate(value))
      {
        Error(kErr_AddrOfNonConstantIndex).Raise(&ln->GetLoc());
      }
    }
  }
  return new AddrOfExpr(var, value, ln->GetLoc());
}

Expr *MakeDerefExpr(const Symbol *name, LocationNode *ln)
{
  Deleter<LocationNode> d(ln);
  bool array, ptr, stack;

  const RCX_Target *t = gProgram->GetTarget();

  if (!t->SourceWritable(kRCX_IndirectType))
    Error(kErr_NoTargetIndirect, t->fName).Raise(&ln->GetLoc());

  int var = gProgram->GetVar(name, array, ptr, stack);
  if (var == kIllegalVar)
  {
          Error(kErr_UndefinedVar, name->GetKey()).Raise(&ln->GetLoc());
  }
  else if (array)
  {
          Error(kErr_VarIsArray, name->GetKey()).Raise(&ln->GetLoc());
  }
  else if (!ptr)
  {
          Error(kErr_VarIsNotPointer, name->GetKey()).Raise(&ln->GetLoc());
  }

  return new DerefExpr(var, ln->GetLoc());
}



DeclareStmt *MakeDeclareStmt(const Symbol *name, LocationNode *ln, Expr *arraySize, bool ptr, bool stack)
{
	Deleter<LocationNode> d(ln);

	int var = gProgram->CreateVar(name, arraySize, ptr, stack);

	int size = 1;
	if (arraySize)
	{
		const RCX_Target *t = gProgram->GetTarget();

		if (!t->fArrays)
			Error(kErr_NoTargetArrays, t->fName).Raise(&ln->GetLoc());

		if (!arraySize->Evaluate(size))
		{
			Error(kErr_ConstantNeeded).Raise(&arraySize->GetLoc());
		}
		delete arraySize;
	}
        if (ptr)
        {
        }


	return new DeclareStmt(name, var, ln->GetLoc(), size, ptr, stack);
}
