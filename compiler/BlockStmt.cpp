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
#include "BlockStmt.h"

BlockStmt::BlockStmt()
{
}


BlockStmt::~BlockStmt()
{
    Stmt *s;

    while ((s = fList.RemoveHead()) != nil)
        delete s;
}


Stmt* BlockStmt::GetChildren()
{
    return fList.GetHead();
}


void BlockStmt::Add(Stmt *s)
{
    if (s) {
        fList.InsertTail(s);
        Adopt(s);
    }
}


void BlockStmt::Prepend(Stmt *s)
{
    if (s) {
        fList.InsertHead(s);
        Adopt(s);
    }
}


void BlockStmt::EmitActual(Bytecode &b)
{
    for(Stmt *s = fList.GetHead(); s; s = s->GetNext()) {
        s->Emit(b);
    }
}


Stmt* BlockStmt::CloneActual(Mapping *b) const
{
    BlockStmt *block = new BlockStmt;

    for(Stmt *s = fList.GetHead(); s; s = s->GetNext()) {
        Stmt *child = s->Clone(b);
        block->fList.InsertTail(child);
        block->Adopt(child);
    }

    return block;
}
