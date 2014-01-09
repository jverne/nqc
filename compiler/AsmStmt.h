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

#ifndef __AsmStmt_h
#define __AsmStmt_h

#ifndef __Stmt_h
#include "Stmt.h"
#endif

#ifndef __RCX_Constants_h
#include "RCX_Constants.h"
#endif

#include <vector>

using std::vector;

class Field;
class Expr;


class Field : public PLinkS<Field>, public AutoFree
{
public:
			Field(Expr *e) : fExpr(e) {}
	virtual	~Field();

	virtual Field*	Clone(Mapping *m) const = 0;
	virtual void	PreEmit(Bytecode &)		{};
	virtual void	Emit(Bytecode &b, vector<UByte> &v) const = 0;

	Expr*			GetExpr() { return fExpr; }

protected:
	Expr*	fExpr;
};


class AsmStmt : public LeafStmt
{
public:
				~AsmStmt();

	void		EmitActual(Bytecode &b);
	Stmt*		CloneActual(Mapping *b) const;

	void		Add(Field *f)	{ fFields.InsertTail(f); }

	virtual void	GetExprs(vector<Expr*> & v) const;

private:
	PListS<Field>	fFields;
};




class ConstField : public Field
{
public:
			ConstField(Expr *e) : Field(e) {};

	virtual Field*		Clone(Mapping *m) const;
	virtual void		Emit(Bytecode &b, vector<UByte> &v) const;

private:
};


class EAField : public Field
{
public:
		enum
		{
			kSmallOperand =	0x1000000,
			kNoTypeCode =	0x2000000,
			kNoLocal =	0x4000000,

			kSourceMask = 0x00ffffff
		};

			EAField(Expr *e, ULong restrictor=0);

	virtual Field*	Clone(Mapping *m) const;
	virtual void	PreEmit(Bytecode &b);
	virtual void	Emit(Bytecode &b, vector<UByte> &v) const;

private:
	ULong		fRestrictor;
	RCX_Value	fEA;
};


#endif
