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
#ifndef __parser_h
#define __parser_h

// forward class declarations for YYSTYPE
class Fragment;
class Resource;
class Stmt;
class BlockStmt;
class Symbol;
class Condition;
class Expr;
class Buffer;
class FunctionDef;
class CallStmt;
class AsmStmt;
class Field;
class CaseStmt;
class DeclareStmt;
class LocationNode;

// get definition of YYSTYPE, yylval, etc
#ifndef __PARSE_TAB_H
#include "parse.tab.h"
int	yylex(YYSTYPE &tokenVal);
#endif

#ifndef _STDIO_H
#include <cstdio>
#endif

// from parse.tab.c
int yyparse(void);

#ifndef __LexLocation_h
#include "LexLocation.h"
#endif

void LexCurrentLocation(LexLocation &loc);
int LexFindAndPushFile(const char *name);
int LexPush(Buffer *buf);
void LexReturnWhitespace(int mode);
void LexReset();

#endif
