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
#ifndef __Macro_h
#define __Macro_h

#ifndef __Token_h
#include "Token.h"
#endif

#ifndef __PTypes_h
#include "PTypes.h"
#endif

class Macro
{
public:
    enum {
        kNoArgs = -1
    };

    Macro(const Token *tokens, int tokenCount, int argCount);
    ~Macro();

    int GetTokenCount() const { return fTokenCount; }
    const Token* GetTokens() const { return fTokens; }
    int GetArgCount() const { return fArgCount; }

    bool IsMarked() const { return fMark; }
    void SetMark() { fMark = true; }
    void ClearMark() { fMark = false; }

private:
    Token* fTokens;
    int fTokenCount;
    int fArgCount;
    bool fMark;
};

#endif
