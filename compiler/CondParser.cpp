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

#include <cstring>
#include <cstdio>
#include "CondParser.h"
#include "Symbol.h"
#include "PreProc.h"
#include "Error.h"

using std::memcpy;

static int Precedence(int t);
static long BinOp(long x, int op, long y);

#define kDefaultDepth	2

CondParser::CondParser()
{
	fMaxDepth = kDefaultDepth;
	fStack = new long[kDefaultDepth];
	fDefinedSymbol = nil;
}

CondParser::~CondParser()
{
	delete [] fStack;
}


bool CondParser::Parse(long &value)
{
	int t;
	TokenVal v;

	Init();
	value = 0;	// default value in case of error

	while((t=gPreProc->GetReplacedToken(v)) != 0)
	{
		bool ok;

		if (t==NL) break;

		if (fState == kValueState)
			ok = DoValueState(t, v);
		else
			ok = DoOpState(t);

		if (!ok)
		{
			Error(kErr_CondExpression).RaiseLex();
			return false;
		}
	}

	if (fState == kValueState)
	{
		Error(kErr_UnexpectedEOL).RaiseLex();
		return false;
	}

	if (fParen != 0)
	{
		Error(kErr_UnbalancedParens).RaiseLex();
		return false;
	}

	Reduce();
	value = fStack[0];

	return true;
}


void CondParser::Init()
{
	fState = kValueState;
	fUnaryCount = 0;
	fDepth = 0;
	fParen = 0;
	if (!fDefinedSymbol)
		fDefinedSymbol = Symbol::Get("defined");
}


bool CondParser::DoValueState(int t, TokenVal v)
{
	long x;

	// only valid tokens are unary operations, '(', ID, and NUMBER
	switch(t)
	{
		case ID:
			if (v.fSymbol == fDefinedSymbol)
			{
				if (!ParseDefined(x))
					return false;
			}
			else
				x = 0;
			ReduceUnaries(x);
			fState = kOpState;
			break;
		case NUMBER:
			ReduceUnaries(v.fInt);
			fState = kOpState;
			break;
		case '-':
		case '!':
		case '~':
			Push(t);
			fUnaryCount++;
			break;
		case '(':
			// push paren state
			Push(fParen);
			Push(fUnaryCount);
			fParen = fDepth;
			fUnaryCount = 0;
			break;
		default:
			return false;
	}

	return true;
}


bool CondParser::DoOpState(int t)
{
	int prec;

	if (t == ')')
	{
		// pop paren state
		long x;
		Reduce();
		x = Pop();
		fUnaryCount = Pop();
		fParen = Pop();
		ReduceUnaries(x);
	}
	else
	{
		prec = Precedence(t);
		if (!prec) return false;

		Reduce(prec);
		Push(t);
		fState = kValueState;
	}

	return true;
}


bool CondParser::ParseDefined(long &value)
{
	int t;
	TokenVal v;

	t = gPreProc->GetReplacedToken(v);
	if (t != '(') return false;

	t = gPreProc->GetRawToken(v);
	if (t != ID) return false;
	value = v.fSymbol->IsDefined();

	t = gPreProc->GetReplacedToken(v);
	if (t != ')') return false;

	return true;
}


void CondParser::Reduce(int limit)
{
	int prec;
	int i;
	long x, y;
	int op;

	y = Pop();

	// reduce any ops with precedence >= limit
	for(i=fDepth-1; i>fParen; i-=2)
	{
		op = fStack[i];
		prec = Precedence(op);
		if (prec < limit) break;

		x = fStack[i-1];

		Drop(2);

		y = BinOp(x, op, y);
	}

	Push(y);
}


void CondParser::ReduceUnaries(long x)
{
	long op;

	while(fUnaryCount)
	{
		op = Pop();
		switch(op)
		{
			case '-':
				x = -x;
				break;
			case '!':
				x = (x ? 0 : 1);
				break;
			case '~':
				x = ~x;
				break;
		}

		fUnaryCount--;
	}

	Push(x);
}


void CondParser::Push(long x)
{
	if (fDepth == fMaxDepth)
	{
		// grow stack
		int newDepth = fMaxDepth * 2;
		long* newStack = new long[newDepth];

		memcpy(newStack, fStack, sizeof(long) * fMaxDepth);
		delete [] fStack;
		fStack = newStack;
		fMaxDepth = newDepth;
	}

	fStack[fDepth++] = x;
}


long BinOp(long x, int op, long y)
{
	//printf("RED: %d %c %d\n", x, op, y);
	switch(op)
	{
		case '*':
			return x * y;
		case '/':
			return x / y;
		case '%':
			return x % y;
		case '+':
			return x + y;
		case '-':
			return x - y;
		case LEFT:
			return x << y;
		case RIGHT:
			return x >> y;
		case '>':
			return x > y;
		case '<':
			return x < y;
		case REL_GE:
			return x >= y;
		case REL_LE:
			return x <= y;
		case REL_EQ:
			return x == y;
		case REL_NE:
			return x != y;
		case '&':
			return x & y;
		case '^':
			return x ^ y;
		case '|':
			return x | y;
		case AND:
			return x && y;
		case OR:
			return x || y;
	}

	// should never get here
	return 0;
}


int Precedence(int t)
{
	switch(t)
	{
		case '*':
		case '/':
		case '%':
			return 10;

		case '+':
		case '-':
			return 9;

		case LEFT:
		case RIGHT:
			return 8;

		case '>':
		case '<':
		case REL_GE:
		case REL_LE:
			return 7;

		case REL_EQ:
		case REL_NE:
			return 6;

		case '&':
			return 5;

		case '^':
			return 4;

		case '|':
			return 3;

		case AND:
			return 2;

		case OR:
			return 1;

		default:
			return 0;
	}
}
