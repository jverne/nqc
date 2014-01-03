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

#include "PListS.h"

void P_ListS::InsertHead(P_LinkS* link)
{
	link->fNext = fHead;
	fHead = link;
	if (!fTail)
		fTail = link;
}


void P_ListS::InsertTail(P_LinkS* link)
{
	link->fNext = nil;
	if (fTail)
		fTail->fNext = link;
	else
		fHead = link;
	fTail = link;
}


P_LinkS *P_ListS::_RemoveHead()
{
	P_LinkS *link = fHead;

	if (link)
	{
		fHead = link->fNext;
		link->fNext = nil;
		if (fHead==nil)
			fTail = nil;
	}

	return link;
}


P_LinkS* P_ListSS::_RemoveHead()
{
	P_LinkS *link = fHead;

	if (link)
	{
		fHead = link->fNext;
		link->fNext = nil;
	}

	return link;
}


void P_ListSS::InsertHead(P_LinkS* link)
{
	link->fNext = fHead;
	fHead = link;
}


bool P_ListSS::Remove(P_LinkS *link)
{
	P_LinkS* prev = nil;
	P_LinkS* l;

	for(l=fHead; l; l=l->fNext)
	{
		if (l==link)
		{
			// unlink and return
			if (prev)
				prev->fNext = l->fNext;
			else
				fHead = l->fNext;
			l->fNext = nil;
			return true;
		}
		prev = l;
	}

	return false;
}


bool P_ListS::Remove(P_LinkS *link)
{
	P_LinkS* prev = nil;
	P_LinkS* l;

	for(l=fHead; l; l=l->fNext)
	{
		if (l==link)
		{
			// unlink and return
			if (prev)
				prev->fNext = l->fNext;
			else
				fHead = l->fNext;
			if (fTail==link)
				fTail = prev;
			l->fNext = nil;
			return true;
		}
		prev = l;
	}

	return false;
}
