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
#ifndef __AcquireStmt_h
#define __AcquireStmt_h

#ifndef __LexLocation_h
#include "LexLocation.h"
#endif

#ifndef __Stmt_h
#include "Stmt.h"
#endif

class AcquireStmt : public BinaryStmt
{
public:
			AcquireStmt(UByte resources, Stmt *body, Stmt *handler, const LexLocation &loc);

	void	EmitActual(Bytecode &b);
	Stmt*	CloneActual(Mapping *b) const;

private:
	UByte		fResources;
	LexLocation	fLocation;
};


#endif
