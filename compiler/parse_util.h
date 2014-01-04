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

#ifndef __parse_util_h
#define __parse_util_h

#ifndef __RCX_Constants_h
#include "RCX_Constants.h"
#endif

#ifndef __LocationNode_h
#include "LocationNode.h"
#endif

#ifndef __AutoFree_h
#include "AutoFree.h"
#endif

class Expr;
class Stmt;
class Condition;
class Symbol;
class CaseStmt;
class FunctionDef;
class Fragment;
class ConstField;
class DeclareStmt;
class AssignStmt;

extern void LexCurrentLocation(LexLocation &loc);


extern Expr *MakeBinaryExpr(Expr *lhs, int op, Expr *rhs);
extern Expr *MakeUnaryExpr(int op, Expr *arg);
extern Expr *MakeValueExpr(Expr *e);
extern Expr *MakeIndirectExpr(Expr *src, Expr *idx);

extern int GetConstantValue(const Expr *e);
extern int GetLValue(const Expr *e);

extern Stmt* MakeCall(Symbol *name, const struct LexLocation &loc);

extern CaseStmt* MakeCaseStmt(const Expr *e, const struct LexLocation &loc);
extern void DefineSubArg(Fragment *f, const Symbol *name, int type);
extern Fragment* CreateSubroutine(Fragment *f, Symbol *name, Stmt *body);
extern Fragment* BeginSubWithParams(Fragment *f, Symbol *name);
extern void EndSubWithParams(Fragment *f, Stmt *body, LocationNode *start, LocationNode *end);
extern void DefineArg(FunctionDef *f, const Symbol *name, int type);
extern FunctionDef* BeginFunction(FunctionDef *f, const Symbol *name, bool listing);
extern void EndFunction(FunctionDef *f, Stmt *body, LocationNode *start, LocationNode *end);
extern ConstField* MakeConstField(Expr *e);
extern Expr *MakeVarExpr(const Symbol *name, LocationNode *loc);
extern Expr *MakeTaskIdExpr(LocationNode *ln);
extern Expr *MakeArrayExpr(const Symbol *name, LocationNode *loc, Expr *index);
extern Expr *MakeAddrOfExpr(const Symbol *name, LocationNode *ln, Expr *index);
extern Expr *MakeDerefExpr(const Symbol *name, LocationNode *ln);
extern Expr *MakeIncDecExpr(Expr *lhs, bool inc, bool pre, const LexLocation &loc);
extern Stmt* MakeAcquireStmt(const Expr *e, Stmt *body, Stmt *handler, LocationNode *ln);
extern Stmt* MakeCatchStmt(const Expr *e, Stmt *body, LocationNode *ln);
extern DeclareStmt *MakeDeclareStmt(const Symbol *name, LocationNode *loc, Expr *arraySize, bool ptr, bool stack);
extern void CheckLValue(Expr *lval);
extern Stmt* MakeAssignStmt(Expr *lhs, int op, Expr *rhs);
extern Stmt* MakeAssign2Stmt(Expr *lhs, int op, Expr *rhs);

extern void BeginScope();
extern Stmt* EndScope(Stmt *body);

#endif
