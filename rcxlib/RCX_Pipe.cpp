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
 /**
  * @file RCX_Pipe.cpp
  * @brief Superclass implementation of LEGO IR USB tower pipe
  */

#include "RCX_Pipe.h"
#include <cstdio>

void RCX_Pipe::FlushRead(int delay)
{
    const int BUFFSIZE = 512;
    UByte * buff = new UByte[BUFFSIZE];

    while (Read(buff, BUFFSIZE, delay) > 0) {;}

    delete[] buff;
}
