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
 * Portions created by David Baum are Copyright (C) 1998 David Baum.
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
