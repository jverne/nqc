/* A Bison parser, made by GNU Bison 1.875.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     OR = 258,
     AND = 259,
     REL_NE = 260,
     REL_EQ = 261,
     REL_GE = 262,
     REL_LE = 263,
     RIGHT = 264,
     LEFT = 265,
     INDIRECT = 266,
     UMINUS = 267,
     INCDEC = 268,
     EVENT_SRC = 269,
     TYPE = 270,
     SIGN = 271,
     ABS = 272,
     LOWER_THAN_ELSE = 273,
     ELSE = 274,
     LOWER_THAN_EXPR_SHIFT = 275,
     ID = 276,
     NUMBER = 277,
     ASSIGN = 278,
     ASSIGN2 = 279,
     TASKOP = 280,
     JUMP = 281,
     TASK = 282,
     SUB = 283,
     STRING = 284,
     PP_DEFINE = 285,
     PP_INCLUDE = 286,
     NL = 287,
     WS = 288,
     PP_ARG = 289,
     PP_UNKNOWN = 290,
     PP_IFDEF = 291,
     PP_IF = 292,
     PP_ELSE = 293,
     PP_ELIF = 294,
     PP_ENDIF = 295,
     PP_UNDEF = 296,
     PP_PRAGMA = 297,
     PP_GLOM = 298,
     PP_ERROR = 299,
     PP_WARNING = 300,
     IF = 301,
     WHILE = 302,
     DO = 303,
     FOR = 304,
     REPEAT = 305,
     SWITCH = 306,
     CASE = 307,
     DEFAULT = 308,
     MONITOR = 309,
     CATCH = 310,
     ACQUIRE = 311,
     GOTO = 312,
     ASM = 313,
     INT = 314,
     T_VOID = 315,
     T_CONST = 316,
     SENSOR = 317,
     TASKID = 318,
     NOLIST = 319,
     RES = 320,
     CTRUE = 321,
     CFALSE = 322
   };
#endif
#define OR 258
#define AND 259
#define REL_NE 260
#define REL_EQ 261
#define REL_GE 262
#define REL_LE 263
#define RIGHT 264
#define LEFT 265
#define INDIRECT 266
#define UMINUS 267
#define INCDEC 268
#define EVENT_SRC 269
#define TYPE 270
#define SIGN 271
#define ABS 272
#define LOWER_THAN_ELSE 273
#define ELSE 274
#define LOWER_THAN_EXPR_SHIFT 275
#define ID 276
#define NUMBER 277
#define ASSIGN 278
#define ASSIGN2 279
#define TASKOP 280
#define JUMP 281
#define TASK 282
#define SUB 283
#define STRING 284
#define PP_DEFINE 285
#define PP_INCLUDE 286
#define NL 287
#define WS 288
#define PP_ARG 289
#define PP_UNKNOWN 290
#define PP_IFDEF 291
#define PP_IF 292
#define PP_ELSE 293
#define PP_ELIF 294
#define PP_ENDIF 295
#define PP_UNDEF 296
#define PP_PRAGMA 297
#define PP_GLOM 298
#define PP_ERROR 299
#define PP_WARNING 300
#define IF 301
#define WHILE 302
#define DO 303
#define FOR 304
#define REPEAT 305
#define SWITCH 306
#define CASE 307
#define DEFAULT 308
#define MONITOR 309
#define CATCH 310
#define ACQUIRE 311
#define GOTO 312
#define ASM 313
#define INT 314
#define T_VOID 315
#define T_CONST 316
#define SENSOR 317
#define TASKID 318
#define NOLIST 319
#define RES 320
#define CTRUE 321
#define CFALSE 322




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 41 "parse.y"
typedef union YYSTYPE {
	int			fInt;
	bool		fBool;
	Resource*	fResource;
	Fragment*	fFragment;
	Stmt*		fStmt;
	BlockStmt*	fBlock;
	Symbol*		fSymbol;
	char*		fString;
	Expr*		fExpr;
	FunctionDef*	fFunction;
	CallStmt*	fCall;
	AsmStmt*	fAsmStmt;
	Field*		fField;
	CaseStmt*	fCaseStmt;
	DeclareStmt*	fDeclareStmt;
	LocationNode*	fLocation;
} YYSTYPE;
/* Line 1248 of yacc.c.  */
#line 189 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



