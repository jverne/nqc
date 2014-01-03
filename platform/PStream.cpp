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

#include "PStream.h"

bool PStream::ReadLine(char *ptr, int max)
{
	bool ok = false;

	while(max > 1)
	{
		if (Read(ptr, 1) != 1) break;
		max--;
		if (*ptr == '\n')
		{
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
