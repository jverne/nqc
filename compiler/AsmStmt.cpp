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
#include "AsmStmt.h"
#include "Bytecode.h"
#include "Expr.h"
#include "RCX_Cmd.h"
#include "RCX_Target.h"
#include "Error.h"

AsmStmt::~AsmStmt()
{
    while (Field *f = fFields.RemoveHead())
        delete f;
}



void AsmStmt::EmitActual(Bytecode &b)
{
    Field *f;

    for (f = fFields.GetHead(); f; f=f->GetNext())
        f->PreEmit(b);

    vector<UByte>v;

    for (f = fFields.GetHead(); f; f=f->GetNext())
        f->Emit(b, v);

    RCX_Cmd cmd;
    cmd.Set(&v[0], v.size());
    b.Add(cmd);
}


Stmt* AsmStmt::CloneActual(Mapping *m) const
{
    AsmStmt *a = new AsmStmt();

    for (Field *f = fFields.GetHead(); f; f=f->GetNext())
        a->Add(f->Clone(m));

    return a;
}


void AsmStmt::GetExprs(vector<Expr *> &v) const
{
    Field *f;

    for (f = fFields.GetHead(); f; f=f->GetNext()) {
        v.push_back(f->GetExpr());
    }
}



Field::~Field()
{
    delete fExpr;
}


Field* ConstField::Clone(Mapping *m) const
{
    return new ConstField(fExpr->Clone(m));
}


void ConstField::Emit(Bytecode &, vector<UByte> &v) const
{
    int value;

    fExpr->Evaluate(value);

    v.push_back( (UByte) value );
}

EAField::EAField(Expr *e, ULong restrictor) :
    Field(e),
    fRestrictor(restrictor),
    fEA(Expr::kIllegalEA)
{
}


Field* EAField::Clone(Mapping *m) const
{
    return new EAField(fExpr->Clone(m), fRestrictor);
}


void EAField::PreEmit(Bytecode &b)
{
    ULong sources = fRestrictor & kSourceMask;
    bool canUseLocal = !(fRestrictor & kNoLocal);

    if (sources || !canUseLocal)
        fEA = fExpr->EmitConstrained(b, sources, canUseLocal);
    else
        fEA = fExpr->EmitAny(b);
}


void EAField::Emit(Bytecode &b, vector<UByte> &v) const
{
    if (!(fRestrictor & kNoTypeCode))
        v.push_back((UByte) RCX_VALUE_TYPE(fEA));

    v.push_back((UByte) RCX_VALUE_DATA(fEA));

    if (!(fRestrictor & kSmallOperand))
        v.push_back((UByte) (RCX_VALUE_DATA(fEA) >> 8));

    b.ReleaseTempEA(fEA);
}
