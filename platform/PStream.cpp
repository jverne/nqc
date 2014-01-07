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
#include "PStream.h"

bool PStream::ReadLine(char *ptr, int max)
{
    bool ok = false;

    while(max > 1) {
        if (Read(ptr, 1) != 1) break;
        max--;
        if (*ptr == '\n') {
            ptr++;
            ok = true;
            break;
        }
        ptr++;
    }

    *ptr = 0;
    return ok;
}


bool PStream::SetTimeout(long timeout_ms)
{
    return (timeout_ms == kPStream_NeverTimeout);
}


bool PStream::SetBlocking(bool blocking)
{
    return SetTimeout(blocking ? kPStream_NeverTimeout : 0);
}


void PStream::FlushWrite()
{
}
