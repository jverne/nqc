%{
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
%}

%{
// prevent redefinition of YYSTYPE in parser.h
#define __PARSE_TAB_H

// these classes must be defined prior to YYSTYPE
#include "Fragment.h"
#include "Resource.h"
#include "BlockStmt.h"
#include "Symbol.h"
#include "Expr.h"
#include "FunctionDef.h"
#include "CallStmt.h"
#include "AsmStmt.h"
#include "CaseStmt.h"
#include "DeclareStmt.h"
#include "Fragment.h"

#if __MWERKS__
#include <alloca.h>
#endif

class Clause;
class LocationNode;

%}

%union {
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
}


%{
#include <stdlib.h>
#include "IfStmt.h"
#include "WhileStmt.h"
#include "AsmStmt.h"
#include "DoStmt.h"
#include "RepeatStmt.h"
#include "AssignStmt.h"
#include "TaskStmt.h"
#include "JumpStmt.h"
#include "SwitchStmt.h"
#include "ExprStmt.h"
#include "RCX_Constants.h"
#include "PreProc.h"
#include "parser.h"
#include "Program.h"
#include "parse_util.h"
#include "Program.h"
#include "IncDecExpr.h"
#include "AtomExpr.h"
#include "TypeExpr.h"
#include "Error.h"
#include "MonitorStmt.h"
#include "EventSrcExpr.h"
#include "SensorExpr.h"
#include "AssignMathStmt.h"
#include "ForStmt.h"
#include "RelExpr.h"
#include "NegateExpr.h"
#include "LogicalExpr.h"
#include "TernaryExpr.h"
#include "LabelStmt.h"
#include "GotoStmt.h"
#include "AddrOfExpr.h"
#include "DerefExpr.h"

#define yylex()		(gPreProc->Get(yylval))

static LexLocation sSavedLoc;

%}

%nonassoc ':' '?'
%left OR
%left AND
%left '|'
%left '^'
%left '&'
%left REL_EQ REL_NE
%left '<' '>'  REL_LE REL_GE
%left LEFT RIGHT
%left '-' '+'
%left '*' '/' '%'
%right UMINUS '~' INDIRECT '!'
%left INCDEC

%nonassoc ABS SIGN TYPE EVENT_SRC
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%nonassoc LOWER_THAN_EXPR_SHIFT
%nonassoc ')'

%token <fSymbol> ID
%token <fInt> NUMBER ASSIGN ASSIGN2 TASKOP JUMP TASK SUB INCDEC
%token <fString> STRING
%token REL_GE REL_LE REL_EQ REL_NE

%token PP_DEFINE PP_INCLUDE NL WS PP_ARG PP_UNKNOWN PP_IFDEF PP_IF PP_ELSE PP_ELIF PP_ENDIF PP_UNDEF
%token PP_PRAGMA PP_GLOM PP_ERROR PP_WARNING
%token IF WHILE DO FOR REPEAT SWITCH CASE DEFAULT MONITOR CATCH ACQUIRE GOTO
%token ASM
%token INT T_VOID T_CONST SENSOR TASKID NOLIST RES
%token CTRUE CFALSE

%type <fStmt> stmt block var_item handler evt_handler opt_handler control_stmt expr_stmt misc_stmt opt_expr_stmt
%type <fDeclareStmt> var_decl
%type <fBlock> stmt_list var_list handler_list
%type <fInt> S arg_type res_byte
%type <fBool> nolist_opt
%type <fExpr> expr opt_expr
%type <fFunction> args arg_list function_head
%type <fCall> params param_list
%type <fAsmStmt> asm_list
%type <fField> asm_item
%type <fCaseStmt> case
%type <fLocation> loc
%type <fFragment> fragment sub_head subfragment sarg_list
%type <fResource> res_data resource

%{
static void yyerror(const char *msg);

%}


%%

S : unit_list	{ $$ = 0; }
	;


unit_list : unit_list unit
	|
	;

unit :	INT var_list ';'		{ gProgram->AddGlobalDecls($2); }
	|	loc fragment loc		{ $2->SetLocations($1, $3); }
	|	loc subfragment loc		{ $2->SetLocations($1, $3); }
	|	loc sub_head '{' stmt_list '}' loc	{ EndSubWithParams($2, $4, $1, $6); }
	|	loc function_head '{' stmt_list '}' loc { EndFunction($2, $4, $1, $6); }
	|	resource ';' { gProgram->AddResource($1); }
	;


function_head : nolist_opt T_VOID ID '(' args ')' { $$ = BeginFunction($5, $3, $1); }
	;

sub_head : SUB ID '(' sarg_list ')' { $$ = BeginSubWithParams($4, $2); }
	;

nolist_opt : NOLIST	{ $$ = 0; }
	|	{ $$ = 1; }
	;

fragment :	TASK ID  '(' ')'  block	{ $$ = new Fragment(true, $2, $5); }
	;

//subfragment :	SUB ID '(' sarg_list ')' '{' stmt_list '}'	{ $$ = CreateSubroutine($4, $2, $7); }
//	|	SUB ID '(' ')' block	        { $$ = new Fragment(false, $2, $5); }
//	;

subfragment :	SUB ID '(' ')' block	        { $$ = new Fragment(false, $2, $5); }
	;

sarg_list : sarg_list ',' arg_type ID	{ $$ = $1; 	DefineSubArg($$, $4, $3); }
	| arg_type ID					{ $$ = new Fragment(false); DefineSubArg($$,$2,$1); }
	;

args : arg_list
	| T_VOID	{ $$ = new FunctionDef(); }
	|			{ $$ = new FunctionDef(); }
	;

arg_list : arg_list ',' arg_type ID	{ $$ = $1; 	DefineArg($$, $4, $3); }
	| arg_type ID					{ $$ = new FunctionDef(); DefineArg($$,$2,$1); }
	;

arg_type : INT		{ $$ = FunctionDef::kIntegerArg; }
	| T_CONST INT 	{ $$ = FunctionDef::kConstantArg; }
	| INT '&'		{ $$ = FunctionDef::kReferenceArg; }
	| T_CONST INT '&'	{ $$ = FunctionDef::kConstRefArg; }
	| SENSOR		{ $$ = FunctionDef::kSensorArg; }
	| INT '*'		{ $$ = FunctionDef::kPointerArg; }
	| T_CONST INT '*'	{ $$ = FunctionDef::kConstPtrArg; }
	;


var_list : var_list ',' var_item	{ $1->Add($3); $$ = $1; }
	| var_item						{ $$ = new BlockStmt(); $$->Add($1); }
	;

var_item : var_decl		{ $$ = $1; }
	| var_decl '=' expr	{ $$ = $1; $1->SetInitialValue($3); }
	;

var_decl : ID loc { $$ = MakeDeclareStmt($1, $2, 0, false, false); }
	| ID loc '[' expr ']'{ $$ = MakeDeclareStmt($1, $2, $4, false, false); }
	| '*' ID loc  { $$ = MakeDeclareStmt($2, $3, 0, true, false); }
	;

block : '{' { BeginScope(); } stmt_list '}'		{ $$ = EndScope($3); }
	;

stmt_list : stmt_list  stmt 	{ $1->Add($2); $$ = $1; }
	|							{ $$ = new BlockStmt(); }
	;

stmt
	:	';'				{ $$ = new BlockStmt(); }
	|	block
	|	case stmt		{ $$ = $1; $1->SetStmt($2); }
	|	loc ID ':' stmt	{ $$ = new LabelStmt($2, $1->GetLoc(), $4); delete $1; }
	|	error ';'		{  yyerrok; $$ = new BlockStmt(); }
	|	control_stmt
	|	misc_stmt loc	{ $1->SetLocation($2); }
	;


control_stmt
	:	WHILE '(' expr ')' loc stmt					{ $$ = new WhileStmt($3, $6); $$->SetLocation($5); }
	|	DO loc stmt WHILE '(' expr ')' ';'		{ $$ = new DoStmt($6, $3); $$->SetLocation($2); }
	|	REPEAT '(' expr ')' loc stmt				{ $$ = new RepeatStmt($3, $6); $$->SetLocation($5); }
	|	SWITCH '(' expr ')' loc stmt				{ $$ = new SwitchStmt($3, $6); $$->SetLocation($5); }
	|	IF	'(' expr ')' loc stmt	%prec LOWER_THAN_ELSE	{ $$ = new IfStmt($3, $6); $$->SetLocation($5); }
	|	IF '(' expr ')' loc stmt ELSE stmt		{ $$ = new IfStmt($3, $6, $8); $$->SetLocation($5); }
	|	MONITOR loc '(' expr ')' loc block handler_list	{ $$ = new MonitorStmt($4, $7, $8, $2->GetLoc()); delete $2; $$->SetLocation($6); }
	|	ACQUIRE loc '(' expr ')' loc block opt_handler	{ $$ = MakeAcquireStmt($4, $7, $8, $2); $$->SetLocation($6); }
	|	FOR '(' opt_expr_stmt ';' opt_expr ';' opt_expr_stmt ')' loc stmt	{ $$ = new ForStmt($3, $5, $7, $10); $$->SetLocation($9); }
	|	GOTO ID saveloc ';' { $$ = new GotoStmt($2, sSavedLoc); }
	;

misc_stmt
	:	ASM '{' asm_list '}' ';'	{ $$ = $3; }
	|	saveloc JUMP ';'			{ $$ = new JumpStmt($2, sSavedLoc); }
	|	TASKOP saveloc ID ';'  		{ $$ = new TaskStmt((UByte)$1, $3, sSavedLoc); }
	|	loc ID '(' params ')' ';'	{ $$ = $4; $4->SetName($2); $4->SetLocation($1->GetLoc()); delete $1; }
	|	INT	var_list ';'			{ $$ = $2; }
	|	expr_stmt ';'
	;

expr_stmt
	:	expr					{ $$ = new ExprStmt($1); }
	|	expr ASSIGN expr		{ CheckLValue($1); $$ = MakeAssignStmt($1, $2, $3); }
	|	expr ASSIGN2 expr		{ CheckLValue($1); $$ = MakeAssign2Stmt($1, $2, $3); }
	|	expr '=' expr			{ CheckLValue($1); $$ = new AssignStmt($1, $3); }
	;


opt_expr_stmt : expr_stmt
  	|				{ $$ = 0; }
	;

opt_expr : expr
	| 	{ $$ = 0; }
	;


opt_handler : handler
	|	{ $$ = 0; }
	;


handler_list :	handler_list evt_handler	{ $1->Add($2); $$ = $1; }
	| 										{ $$ =  new BlockStmt(); }
	;

evt_handler : CATCH block		{ $$ = $2; }
	| CATCH loc '(' expr ')' block	{ $$ = MakeCatchStmt($4, $6, $2); }
	;


handler : CATCH block	{ $$ = $2; }
	;

case  : saveloc CASE expr ':'	{ $$ = MakeCaseStmt($3, sSavedLoc); }
	  | saveloc DEFAULT ':'		{ $$ = new CaseStmt(CaseStmt::kDefaultValue, sSavedLoc); }
	  ;

params : param_list
	|				{ $$ = new CallStmt(); }
	;

param_list : param_list ',' expr	{ $$ = $1; $$->AddParam($3); }
	| expr							{ $$ = new CallStmt(); $$->AddParam($1); }
	;

expr : NUMBER saveloc		{ $$ = new AtomExpr(kRCX_ConstantType, $1, sSavedLoc); }
	|	expr '+' expr	{ $$ = MakeBinaryExpr($1, '+', $3); }
	|	expr '-' expr	{ $$ = MakeBinaryExpr($1, '-', $3); }
	|	expr '*' expr	{ $$ = MakeBinaryExpr($1, '*', $3); }
	|	expr '/' expr	{ $$ = MakeBinaryExpr($1, '/', $3); }
	|	expr '&' expr	{ $$ = MakeBinaryExpr($1, '&', $3); }
	|	expr '|' expr	{ $$ = MakeBinaryExpr($1, '|', $3); }
	|	expr '%' expr	{ $$ = MakeBinaryExpr($1, '%', $3); }
	|	expr LEFT expr	{ $$ = MakeBinaryExpr($1, LEFT, $3); }
	|	expr RIGHT expr	{ $$ = MakeBinaryExpr($1, RIGHT, $3); }
	|	expr '^' expr	{ $$ = MakeBinaryExpr($1, '^', $3); }
	|	expr REL_EQ expr	{ $$ = new RelExpr($1, RelExpr::kEqualTo, $3); }
	|	expr REL_LE expr	{ $$ = new RelExpr($1, RelExpr::kLessOrEqual, $3); }
	|	expr REL_GE expr	{ $$ = new RelExpr($1, RelExpr::kGreaterOrEqual, $3); }
	|	expr REL_NE expr	{ $$ = new RelExpr($1, RelExpr::kNotEqualTo, $3); }
	|	expr '<' expr	{ $$ = new RelExpr($1, RelExpr::kLessThan, $3); }
	|	expr '>' expr	{ $$ = new RelExpr($1, RelExpr::kGreaterThan, $3); }
	|	CTRUE saveloc			{ $$ = new AtomExpr(kRCX_ConstantType, 1, sSavedLoc); }
	|	CFALSE saveloc			{ $$ = new AtomExpr(kRCX_ConstantType, 0, sSavedLoc); }
	|	'!' expr		{ $$ = new NegateExpr($2); }
	|	expr AND expr	{ $$ = new LogicalExpr($1, LogicalExpr::kLogicalAnd, $3); }
	|	expr OR expr	{ $$ = new LogicalExpr($1, LogicalExpr::kLogicalOr, $3); }

	|	'-'	expr %prec UMINUS	{ $$ = MakeBinaryExpr(new AtomExpr(kRCX_ConstantType, 0, $2->GetLoc()), '-', $2); }
	|	'~' expr %prec UMINUS	{ $$ = MakeUnaryExpr('~', $2); }

	|	ABS '(' expr ')' 	{ $$ = MakeUnaryExpr(ABS, $3); }
	|	SIGN '(' expr ')'	{ $$ = MakeUnaryExpr(SIGN, $3); }

	| '(' expr ')'			{ $$ = $2; }
	| loc ID 				{ $$ = MakeVarExpr($2, $1); }
	| loc ID '[' expr ']'	{ $$ = MakeArrayExpr($2, $1, $4); }
	| '@' expr %prec UMINUS			{ $$ = MakeValueExpr($2); }

	| expr INCDEC				{ $$ = MakeIncDecExpr($1, $2, false, $1->GetLoc()); }
	| INCDEC expr %prec UMINUS	{ $$ = MakeIncDecExpr($2, $1, true, $2->GetLoc()); }

	| SENSOR '(' expr ')'		{ $$ = new SensorExpr($3); }
	| TYPE '(' expr ')'			{ $$ = new TypeExpr($3); }
	| EVENT_SRC '(' expr ')'	{ $$ = new EventSrcExpr($3, gProgram->GetTarget()->fType); }
	| loc TASKID				{ $$ = MakeTaskIdExpr($1); }
	| INDIRECT '(' expr ')' '[' expr ']'		{ $$ = MakeIndirectExpr($3, $6); }
	| expr '?' expr ':' expr	{ $$ = new TernaryExpr($1, $3, $5); }
        | loc '&' ID                              { $$ = MakeAddrOfExpr($3, $1, (Expr *)0); }
        | loc '&' ID '[' expr ']'                 { $$ = MakeAddrOfExpr($3, $1, $5); }
        | loc '*' ID                              { $$ = MakeDerefExpr($3, $1); }
	;

saveloc :  %prec LOWER_THAN_EXPR_SHIFT		{ LexCurrentLocation(sSavedLoc); }
	;

loc : %prec LOWER_THAN_EXPR_SHIFT	{ $$ = new LocationNode(); }
	;

asm_list : asm_list ',' asm_item	{ $$ = $1; $1->Add($3); }
	| asm_item	{ $$ = new AsmStmt(); $$->Add($1); }
	|			{ $$ = new AsmStmt(); }
	;


asm_item : expr		{ $$ = MakeConstField($1); }
	| '$' expr	{ $$ = new EAField($2); }
	| '$' expr ':' expr	{ $$ = new EAField($2, GetConstantValue($4)); }
	;


resource : RES expr ID '{' res_data '}' { $$=$5; $5->SetInfo((RCX_ChunkType)GetConstantValue($2), $3); }
	;

res_data : res_data ',' res_byte	{ $$ = $1; $1->Add($3); }
	|	res_byte	{ $$ = new Resource(); $$->Add($1); }
	|				{ $$ = new Resource(); }
	;

res_byte : expr		{ $$ = GetConstantValue($1); }
	;


%%


void yyerror(const char *msg)
{
	Error(kErr_Parser, msg).RaiseLex();
}

