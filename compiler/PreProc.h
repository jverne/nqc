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
#ifndef __PreProc_h
#define __PreProc_h

#ifndef __Token_h
#include "Token.h"
#endif

#ifndef __PListS_h
#include "PListS.h"
#endif

#ifndef __CondParser_h
#include "CondParser.h"
#endif

#ifndef __Conditional_h
#include "Conditional.h"
#endif

#include <vector>

using std::vector;

class Symbol;
class Expansion;

 /*
  * Reads tokens from the Lexer and emits a preprocessed token stream.
  * The entire process is 'pulled' by calling PreProc::Get(), which in turn calls
  * down through the layers of the Preprocessor.
  */
class PreProc
{
public:
            PreProc();
            ~PreProc();

    /**
     * Get fully preprocessed token.
     * Dispatches directives (DoInclude(), DoDefine(), etc)
     * When processing a directive, GetRawToken() is called - in effect, macro
     * substitution happens on playback, not record.
     */
    int     Get(TokenVal &v);

    /**
     * Get after substitution but before directive processing.
     * Matches ID tokens against macros, and pushes the macro
     * definition when a macro is used.  Use this call when macro calls should
     * be expanded.
     */
    int     GetReplacedToken(TokenVal &v);

    /**
     * Get next queued token (either from macro or file).
     * Maintains the queue of expansions, subsitutes arguments during
     * expansion, manages the lexer (calls yylex(), maintains fNLRead, etc.)
     */
    int     GetRawToken(TokenVal &v);

private:

    bool    DoDefine();
    bool    DoInclude();
    int     DoSimpleDefine(Symbol *s);
    int     DoMacroDefine(Symbol *s);
    bool    DoIfdef(bool b);
    bool    DoUndef();
    bool    DoPragma();

    int     ReadDefineArgs();
    void    ReadDefineBody();
    int     MatchArg(const Symbol *s);

    void    DiscardLine();

    bool    BeginExpansion(Symbol *s);
    bool    ReadExpansionArgs(Expansion *e);
    bool    ReadExpansionArg(Expansion *e, int i, int delim);

    bool    fNLRead;

    PListS<Expansion>   fExpList;
    vector<Symbol*>     fArguments;
    vector<Token>       fTokenBuf;
    Conditional         fConditional;
    bool                fActive;
    CondParser          fParser;
    bool                fEndOfFiles;
};


extern PreProc *gPreProc;

#endif
